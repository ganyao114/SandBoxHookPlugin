//
// Created by hluwa on 2018/8/19.
//

#ifndef JNITEST_NATIVE_LIB_H
#define JNITEST_NATIVE_LIB_H

#include <jni.h>
#include <sys/types.h>
#include <android/log.h>

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *);

#define LOGD(...) __android_log_print(ANDROID_LOG_ERROR, "And-Inject", __VA_ARGS__)

#endif //JNITEST_NATIVE_LIB_H
