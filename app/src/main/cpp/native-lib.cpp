#include <jni.h>
#include <string>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include "native-lib.h"
#include <dirent.h>
#include <dlfcn.h>
#include "Inject.h"
#include "fake_dlfcn.h"

JavaVM *gJavaVM = NULL;
JNIEnv *gJNIEnv = NULL;
jobject sClassLoader = NULL;


int find_pid_of(const char *process_name) {
    int id;
    pid_t pid = -1;
    DIR *dir;
    FILE *fp;
    char filename[32];
    char cmdline[256];

    struct dirent *entry;

    if (process_name == NULL)
        return -1;

    dir = opendir("/proc/");
    if (dir == NULL)
        return -1;

    while ((entry = readdir(dir)) != NULL) {
        id = atoi(entry->d_name);
        if (id != 0) {
            sprintf(filename, "/proc/%d/cmdline", id);
            fp = fopen(filename, "r");
            if (fp) {
                fgets(cmdline, sizeof(cmdline), fp);
                fclose(fp);

                if (strcmp(process_name, cmdline) == 0) {
                    /* process found */
                    pid = id;
                    break;
                }
            }
        }
    }

    closedir(dir);

    LOGD("target-pid %d", pid);

    return pid;
}


jint (*GetCreatedJavaVMs)(JavaVM **, jsize, jsize *) = NULL;

static void init_gvar() {
#ifdef __aarch64__
#define ART_PATH "/system/lib64/libart.so"
#define DVM_PATH "/system/lib64/libdvm.so"
#else
#define ART_PATH "/system/lib/libart.so"
#define DVM_PATH "/system/lib/libdvm.so"
#endif

    __android_log_print(ANDROID_LOG_DEBUG, "jnitest", "init");
    jsize size = 0;
    void *handle = NULL;
    if (access(DVM_PATH, F_OK) == 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "jnitest", "init_dvm");
        handle = fake_dlopen(DVM_PATH, RTLD_NOW | RTLD_GLOBAL);
    } else if (access(ART_PATH, F_OK) == 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "jnitest", "init_art");
        handle = fake_dlopen(ART_PATH, RTLD_NOW | RTLD_GLOBAL);
    }
    if (!handle) {
        return;
    }
    __android_log_print(ANDROID_LOG_DEBUG, "jnitest", "init_dlopen VM => %p", handle);
    GetCreatedJavaVMs = (jint (*)(JavaVM **, jsize, jsize *)) fake_dlsym(handle,
                                                                    "JNI_GetCreatedJavaVMs");
    if (!GetCreatedJavaVMs) {
        return;
    }
    __android_log_print(ANDROID_LOG_DEBUG, "jnitest", "init_dlsym GetCreatedJavaVMs => %p",
                        GetCreatedJavaVMs);
    GetCreatedJavaVMs(&gJavaVM, 1, &size);
    if (size >= 1) {
        __android_log_print(ANDROID_LOG_DEBUG, "jnitest", "init_GetCreatedJavaVMs => %p", &gJavaVM);
        gJavaVM->GetEnv((void **) &gJNIEnv, JNI_VERSION_1_6);
        if (gJNIEnv) {
            __android_log_print(ANDROID_LOG_DEBUG, "jnitest", "init_GetEnv => %p", &gJNIEnv);
            jclass dex_class_loader = gJNIEnv->FindClass("java/lang/ClassLoader");
            if (dex_class_loader) {
                __android_log_print(ANDROID_LOG_DEBUG, "jnitest",
                                    "init_FindClass ClassLoader => %p", &dex_class_loader);
                jmethodID get_system_class_loader = gJNIEnv->GetStaticMethodID(dex_class_loader,
                                                                               "getSystemClassLoader",
                                                                               "()Ljava/lang/ClassLoader;");
                if (get_system_class_loader) {
                    __android_log_print(ANDROID_LOG_DEBUG, "jnitest",
                                        "init_GetMethodID getSystemClassLoader => %p",
                                        get_system_class_loader);
                    sClassLoader = gJNIEnv->CallStaticObjectMethod(dex_class_loader,
                                                                   get_system_class_loader);
                    __android_log_print(ANDROID_LOG_DEBUG, "jnitest",
                                        "init_CallMethod getSystemClassLoader=> %p", &sClassLoader);
                }
            }
        }
    }
}

jobject load_module(char *filepath) {

    if (sClassLoader) {
        jclass path_class_loader = gJNIEnv->FindClass("dalvik/system/PathClassLoader");
        if (path_class_loader) {
            jmethodID cort = gJNIEnv->GetMethodID(path_class_loader, "<init>",
                                                  "(Ljava/lang/String;Ljava/lang/ClassLoader;)V");
            if (cort) {
                return gJNIEnv->NewObject(path_class_loader, cort, gJNIEnv->NewStringUTF(filepath),
                                          sClassLoader);
            }

        }
    }
    return NULL;
}

int entry() {
    __android_log_print(ANDROID_LOG_DEBUG, "jnitest", "entry_3");
    init_gvar();
    jobject loader = load_module("/data/app/com.swift.hookdemo-1/base.apk");
    __android_log_print(ANDROID_LOG_DEBUG, "jnitest", "loader = %p", loader);
    jclass clazz = gJNIEnv->FindClass("java/lang/ClassLoader");
    __android_log_print(ANDROID_LOG_DEBUG, "jnitest", "clazz = %p", clazz);
    jmethodID forclass = gJNIEnv->GetMethodID(clazz, "loadClass",
                                              "(Ljava/lang/String;)Ljava/lang/Class;");
    __android_log_print(ANDROID_LOG_DEBUG, "jnitest", "loadClass = %p", forclass);
    if (!forclass) {
        return 0x100;
    }
    jstring hookStub = gJNIEnv->NewStringUTF("com.swift.hookdemo.HookStub");
    jclass cls = static_cast<jclass>(gJNIEnv->CallObjectMethod(loader, forclass, hookStub));
    jmethodID callback = gJNIEnv->GetStaticMethodID(cls, "onInjected", "()V");
    __android_log_print(ANDROID_LOG_DEBUG, "jnitest", "callback = %p", callback);
    gJNIEnv->CallStaticVoidMethod(cls, callback);
//    jclass cls = gJNIEnv->FindClass("cn/hluwa/injector/MainActivity");

//    gJNIEnv->CallStaticVoidMethod()
    return 0x100;
}

#define __DEBUG__ 0

extern "C"
JNIEXPORT void JNICALL
Java_com_swift_hookdemo_MainActivity_inject(JNIEnv *env, jobject instance, jstring pkgname_,
                                            jstring payload_) {
    const char *pkgname = env->GetStringUTFChars(pkgname_, 0);
    const char *payload = env->GetStringUTFChars(payload_, 0);

    // TODO
    Inject *injector = new Inject(find_pid_of(pkgname));
    injector->call_sym(const_cast<char *>(payload), "_Z5entryv", NULL, 0);
    delete injector;


    env->ReleaseStringUTFChars(pkgname_, pkgname);
    env->ReleaseStringUTFChars(payload_, payload);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_swift_hookdemo_MainActivity_injectbypid(JNIEnv *env, jobject instance,
                                                                  jint pid, jstring payload_) {
    const char *payload = env->GetStringUTFChars(payload_, 0);

    // TODO
    Inject *injector = new Inject(pid);
    injector->call_sym(const_cast<char *>(payload), "_Z5entryv", NULL, 0);
    delete injector;

    env->ReleaseStringUTFChars(payload_, payload);
}