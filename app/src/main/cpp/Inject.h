//
// Created by hluwa on 2018/8/19.
//

#ifndef JNITEST_INJECT_H
#define JNITEST_INJECT_H

#define CPSR_T_MASK     ( 1u << 5 )
#define REMOTE_STR_BUF_SIZE 0x10000
#define HANDLES_LIST_SIZE 128

#ifdef __aarch64__
#define LIBC_PATH "/system/lib64/libc.so"
#define LINKER_PATH "/system/bin/linker64"
#define ARGS_REG_NUM 8
#define pt_regs user_pt_regs
#define PTRACE_SETREGS PTRACE_SETREGSET
#define PTRACE_GETREGS PTRACE_GETREGSET
#define uregs regs
#define ARM_sp sp
#define ARM_pc pc
#define ARM_lr regs[30]
#define ARM_cpsr pstate
#else
#define LIBC_PATH "/system/lib/libc.so"
#define LINKER_PATH "/system/bin/linker"
#define ARGS_REG_NUM 4
#endif

#include <sys/types.h>
#include <android/log.h>


#define LOGD(...) __android_log_print(ANDROID_LOG_ERROR, "And-Inject", __VA_ARGS__)

void *get_module_base(pid_t pid, const char *module_name);

typedef enum {
    HLUWA_STATUS_FAILD,
    HLUWA_STATUS_SUCCESS,
    HLUWA_STATUS_CALL_ADDR,
    HLUWA_STATUS_DLOPEN_ERROR,
    HLUWA_STATUS_DLSYM_ERROR
} inject_status;


class Inject {
public:
    pid_t pid;
    void *remote_buf;

    Inject(pid_t pid);

    ~Inject();

    inject_status status;

    void *call_addr(void *remote_addr, void **args, int argc);

    void *call_sym(char *module, char *sym, void **args, int argc);

    void *get_remote_addr(const char *module_name, void *local_addr);

    void *loadlibrary(char *libfile);

    void *write_string(char *string);

private:
    int buf_cursor;

    int attach();

    int detach();

    void *dlopen_addr;
    void *dlsym_addr;
    void *dlclose_addr;
    int handles_cursor;

    int getregs(struct pt_regs *regs);

    int setregs(struct pt_regs *regs);

    int write_data(uint8_t *dest, uint8_t *data, size_t size);

    int read_data(uint8_t *src, uint8_t *buf, size_t size);

    int read_string(uint8_t *remote_addr, uint8_t *dest, size_t max_len);

    int cont();

    void *handles[HANDLES_LIST_SIZE];
};


#endif //JNITEST_INJECT_H
