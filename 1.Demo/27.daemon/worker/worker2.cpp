/*************************************************************************
    > File Name: worker.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Mon 05 May 2025 10:50:39 AM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <dlfcn.h>
#include "mlib.h"
#include "mlibcpp.h"

#define WORKER_TAG "work2"

volatile sig_atomic_t keep_running = 1;

void handle_signal(int sig)
{
    keep_running = 0;
}

typedef void (*m_lib_func_t)(const char *prefix);

#define USE_DL_OPT

int main(int argc, char *argv[])
{
    void *handle = NULL;
    TestLib *test_lib = TestLib::get_instance();
    m_lib_func_t m_fun = NULL;
    const char *dlsym_error = NULL;
    int run_times = 0;

    // 设置信号处理
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    // 初始化库
#ifndef USE_DL_OPT
    if (lib_init() != 0) {
        fprintf(stderr, "Failed to initialize library\n");
        return EXIT_FAILURE;
    }
#endif

    printf("Worker process started (PID: %d)\n", getpid());

    // 主工作循环
    while (keep_running) {
        /* lib_do_work(WORKER_TAG); */
        test_lib->work(WORKER_TAG);

#ifdef USE_DL_OPT
        /* load dl */
        if (run_times > 3) {
            handle = dlopen("libmlib.so", RTLD_LAZY);
            m_fun = (m_lib_func_t)dlsym(handle, "lib_do_work");
            dlsym_error = dlerror();
            if (dlsym_error) {
                fprintf(stderr, "%s\n", dlsym_error);
                break;
            }
            m_fun(WORKER_TAG);

            dlclose(handle);
        }
#endif

        run_times++;
        sleep(1);
    }

    // 清理
#ifndef USE_DL_OPT
    lib_cleanup();
#endif
    printf("Worker process exiting\n");

    return EXIT_SUCCESS;
}
