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
#include "mlib.h"
#include "mlibcpp.h"

volatile sig_atomic_t keep_running = 1;

void handle_signal(int sig)
{
    keep_running = 0;
}

int main(int argc, char *argv[])
{
    TestLib *test_lib = TestLib::get_instance();

    // 设置信号处理
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    // 初始化库
    if (lib_init() != 0) {
        fprintf(stderr, "Failed to initialize library\n");
        return EXIT_FAILURE;
    }

    printf("Worker process started (PID: %d)\n", getpid());

    // 主工作循环
    while (keep_running) {
        lib_do_work();
        test_lib->work();
    }

    // 清理
    lib_cleanup();
    printf("Worker process exiting\n");

    return EXIT_SUCCESS;
}
