/*************************************************************************
    > File Name: daemon_guard.c
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
#include <string.h>
#include <errno.h>

#define MAX_RESTARTS 5
#define RESTART_DELAY 3

typedef struct {
    char *command;      // 完整命令（含参数）
    char *name;         // 进程标识名
    pid_t pid;          // 当前PID
    int max_restarts;   // 最大重启次数
    int restart_delay;  // 重启延迟(秒)
    int restart_count;  // 已重启次数
    time_t last_restart;
} Worker;

Worker *workers = NULL;
int worker_count = 0;


void cleanup();

/*
 *
 * 守护进程 (daemon_guard.c)
 *   │
 *   └─ 监控并重启 → 多个工作进程 (worker.c)
 *                      │
 *                      └─ 调用 → 依赖库 (mlib.c/mlib.h)
 *
 * 1. 脱离终端控制: 使进程不受用户终端退出影响（比如关闭SSH后进程仍继续运行）
 * 2. 后台运行: 脱离前台交互，作为系统服务长期运行
 * 3. 环境隔离: 创建独立的运行环境，避免继承父进程的某些限制
 */
void daemonize()
{
    // 守护进程化
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

    /* freopen("/dev/null", "r", stdin); */
    /* freopen("/var/log/daemon_guard.log", "a", stdout); */
    /* freopen("/var/log/daemon_guard.err", "a", stderr); */

    // 重设文件权限掩码
    // 目的：
    // 清除从父进程继承的文件权限掩码
    // 确保守护进程创建的文件具有精确的权限（如 0666 允许所有用户读写）
    umask(022);  // 文件默认权限: 644 (rw-r--r--)

    // 注册信号处理
    signal(SIGTERM, cleanup);
    signal(SIGINT, cleanup);

    // 关闭标准文件描述符
    // 目的：
    // 释放继承自父进程的资源
    // 避免后台进程意外读写终端
    // 注：更完善的实现会重定向到 /dev/null 或日志文件。
    /* close(STDIN_FILENO); */
    /* close(STDOUT_FILENO); */
    /* close(STDERR_FILENO); */
}

// 启动工作进程
void start_worker(Worker *w)
{
    // 在父进程中：fork() 返回子进程的 PID（正整数）
    // 在子进程中：fork() 返回 0
    // 如果出错：fork() 返回 -1
    pid_t pid = fork();
    if (pid == 0) {
        // 子进程执行命令
        //
        // int execl(const char *path, const char *arg0, ..., (char *)0);
        //
        // path: 要执行的可执行文件的完整路径
        // arg0: 通常是程序名（argv[0]）
        // ...: 可变参数列表，表示程序的命令行参数
        // (char *)0: 必须以空指针 (NULL) 结尾，表示参数列表结束
        //
        // 进程替换：execl() 不会创建新进程，而是替换当前进程的代码段、数据段等
        // 继承性：新程序继承原进程的：
        //   进程ID（PID）
        //   父进程ID（PPID）
        //   文件描述符（除非设置了 close-on-exec 标志）
        //   信号处理设置
        //   当前工作目录等
        // 参数传递：必须显式列出所有参数，并以 NULL 结束
        execl("/bin/sh", "sh", "-c", w->command, NULL);
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // 父进程记录信息
        w->pid = pid;
        w->last_restart = time(NULL);
        printf("[%s] boot PID=%d\n", w->name, pid);
    }
}

/*
 * waitpid 函数使用方法：
 *
 * 原型：
 * pid_t waitpid(pid_t pid, int *status, int options);
 *
 *
 * 参数      类型    说明
 * pid	     pid_t	 指定要等待的子进程：
 *                     >0：等待进程ID等于 pid 的子进程
 *                     -1：等待任意子进程（同 wait）
 *                     0：等待与调用进程同进程组的任意子进程
 *                     <-1：等待进程组ID等于 `pid` 的任意子进程
 * status    int *   输出子进程的退出状态（可用宏解析，见下文）
 * options   int     控制行为：
 *                     0：阻塞等待
 *                     WNOHANG：非阻塞（无子进程退出时立即返回）
 *                     WUNTRACED：也报告停止的子进程（如被信号暂停）
 *                     WCONTINUED：报告继续执行的子进程
 *
 * 返回值
 *   成功：返回状态变化的子进程PID。
 *   失败：返回 -1（如无目标子进程或信号中断），并设置 errno（如 ECHILD 或 EINTR）。
 *   非阻塞模式（WNOHANG）：若无子进程退出，返回 0。
 *
 *
 * 通过 status 参数可获取子进程退出原因：
 * 宏                   说明
 * WIFEXITED(status)    若子进程正常退出（如调用 exit 或 return）返回真
 * WEXITSTATUS(status)  若 WIFEXITED 为真，提取子进程退出码（exit(3) 中的 3）
 * WIFSIGNALED(status)  若子进程因信号终止返回真
 * WTERMSIG(status)     若 WIFSIGNALED 为真，提取终止信号的编号（如 SIGKILL 为 9）
 * WIFSTOPPED(status)   若子进程因信号暂停返回真（需设置 WUNTRACED 选项）
 * WSTOPSIG(status)     若 WIFSTOPPED 为真，提取暂停信号的编号
 * WIFCONTINUED(status) 若子进程由 SIGCONT 信号恢复执行返回真（需设置 WCONTINUED 选项）
 */

// 监控循环
void monitor_workers()
{
    while (1) {
        for (int i = 0; i < worker_count; i++) {
            Worker *w = &workers[i];
            int status;
            pid_t res = waitpid(w->pid, &status, WNOHANG);

            if (res == w->pid) {
                // 进程终止
                time_t now = time(NULL);
                if (now - w->last_restart < w->restart_delay) {
                    w->restart_count++;
                } else {
                    w->restart_count = 0;
                }

                if (w->restart_count >= w->max_restarts) {
                    fprintf(stderr, "[%s] Exceeded the maximum number of restarts, stop monitoring\n", w->name);
                    continue;
                }

                printf("[%s] proc stoped，after %ds reboot proc...\n", w->name, w->restart_delay);
                sleep(w->restart_delay);
                start_worker(w);
            }
        }
        sleep(1); // 降低CPU占用
    }
}

// 清理资源
void cleanup()
{
    for (int i = 0; i < worker_count; i++) {
        if (workers[i].pid > 0) kill(workers[i].pid, SIGTERM);
        free(workers[i].command);
        free(workers[i].name);
    }
    free(workers);
}

// 参数解析
void parse_args(int argc, char *argv[])
{
    int capacity = 2; // 初始容量
    workers = malloc(capacity * sizeof(Worker));

    for (int i = 1; i < argc; ) {
        if (strcmp(argv[i], "--cmd") == 0 && i+1 < argc) {
            // 扩容检查
            if (worker_count >= capacity) {
                capacity *= 2;
                workers = realloc(workers, capacity * sizeof(Worker));
            }

            Worker *w = &workers[worker_count++];
            w->command = strdup(argv[i+1]);
            w->name = strdup("unnamed");
            w->max_restarts = 3;
            w->restart_delay = 1;
            w->pid = -1;
            i += 2;
        } else if (strcmp(argv[i], "--name") == 0 && i+1 < argc) {
            free(workers[worker_count-1].name);
            workers[worker_count-1].name = strdup(argv[i+1]);
            i += 2;
        } else if (strcmp(argv[i], "--max-restarts") == 0 && i+1 < argc) {
            workers[worker_count-1].max_restarts = atoi(argv[i+1]);
            i += 2;
        } else if (strcmp(argv[i], "--restart-delay") == 0 && i+1 < argc) {
            workers[worker_count-1].restart_delay = atoi(argv[i+1]);
            i += 2;
        } else if (strcmp(argv[i], "--") == 0) {
            i++; // 分隔符，开始下一个worker的定义
        } else {
            fprintf(stderr, "unknow para: %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: %s --cmd \"cmd\" [--name name] [--max-restarts N]"
                " [--restart-delay N] -- [other workers...]\n", argv[0]);
        return 1;
    }

    // 解析参数
    parse_args(argc, argv);
    if (worker_count == 0) {
        fprintf(stderr, "err: No monitoring process is specified\n");
        return 1;
    }

    daemonize();

    // 启动所有worker
    for (int i = 0; i < worker_count; i++) {
        start_worker(&workers[i]);
    }

    // 开始监控
    monitor_workers();

    cleanup();
    return 0;
}
