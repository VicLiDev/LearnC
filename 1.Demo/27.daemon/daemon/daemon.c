/*************************************************************************
    > File Name: daemon.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Mon 05 May 2025 10:46:55 AM CST
 ************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>

#define MAX_RESTARTS 5
#define RESTART_DELAY 3

char *worker_path = NULL;  // 存储worker路径

/*
 *
 * 守护进程 (daemon.c)
 *   │
 *   └─ 监控并重启 → 工作进程 (worker.c)
 *                      │
 *                      └─ 调用 → 依赖库 (mlib.c/mlib.h)
 *
 * 1. 脱离终端控制: 使进程不受用户终端退出影响（比如关闭SSH后进程仍继续运行）
 * 2. 后台运行: 脱离前台交互，作为系统服务长期运行
 * 3. 环境隔离: 创建独立的运行环境，避免继承父进程的某些限制
 */
void daemonize()
{
    // fork() 创建子进程，父进程退出
    // 目的：让子进程成为孤儿进程，被 init 进程（PID=1）接管，脱离原终端控制。
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // 父进程退出

    // 创建新会话
    // 目的：
    // 成为新会话的首进程
    // 脱离原终端的控制（不再响应终端信号如 SIGHUP）
    // 脱离原进程组
    if (setsid() < 0) exit(EXIT_FAILURE);

    // 设置工作目录
    // 目的：
    // 避免占用可卸载的文件系统（如 /mnt）
    // 防止因目录被删除导致路径错误
    chdir("/");

    // 重设文件权限掩码
    // 目的：
    // 清除从父进程继承的文件权限掩码
    // 确保守护进程创建的文件具有精确的权限（如 0666 允许所有用户读写）
    umask(0);

    // 关闭标准文件描述符
    // 目的：
    // 释放继承自父进程的资源
    // 避免后台进程意外读写终端
    // 注：更完善的实现会重定向到 /dev/null 或日志文件。
    /* close(STDIN_FILENO); */
    /* close(STDOUT_FILENO); */
    /* close(STDERR_FILENO); */
}

void start_worker(pid_t *child_pid)
{
    *child_pid = fork();
    if (*child_pid == 0) {
        // 子进程(工作进程)
        execl(worker_path, worker_path, NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
}

void usage(const char *progname)
{
    fprintf(stderr, "Usage: %s <worker_path>\n", progname);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        usage(argv[0]);
    }

    worker_path = argv[1];  // 从命令行参数获取worker路径

    daemonize();

    pid_t child_pid;
    int status;
    int restart_count = 0;
    time_t last_restart = time(NULL);

    start_worker(&child_pid);

    while (1) {
        // 等待子进程结束
        if (waitpid(child_pid, &status, WNOHANG) == child_pid) {
            time_t now = time(NULL);

            // 检查重启频率
            if (now - last_restart < RESTART_DELAY) {
                restart_count++;
            } else {
                restart_count = 0;
            }

            if (restart_count >= MAX_RESTARTS) {
                // 记录到系统日志或文件
                exit(EXIT_FAILURE);
            }

            // 重启工作进程
            sleep(RESTART_DELAY);
            start_worker(&child_pid);
            last_restart = time(NULL);
        }

        sleep(1); // 减少CPU使用
    }

    return EXIT_SUCCESS;
}
