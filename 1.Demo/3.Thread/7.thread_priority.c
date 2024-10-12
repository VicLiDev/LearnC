/*************************************************************************
    > File Name: 7.thread_priority.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Thu 10 Oct 2024 08:37:20 PM CST
 ************************************************************************/

/*
 * 在 Linux 系统中，进程的优先级是通过 "nice" 值来表示的，优先级范围为 -20 到 19。
 *   -20 表示最高优先级（更高的调度优先权）。
 *   19 表示最低优先级（更低的调度优先权）。
 *   进程的默认 "nice" 值通常是 0，这意味着进程的优先级是系统的默认优先级。
 */


#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>

int main()
{
    int prio;
    pid_t pid = getpid(); // 获取当前进程的 PID

    // 获取当前进程的优先级
    /*
     * which: 这个参数指定要获取优先级的类型。常用的值包括：
     *   PRIO_PROCESS：表示获取进程的优先级。
     *   PRIO_PGRP：表示获取进程组的优先级。
     *   PRIO_USER：表示获取某个用户的优先级。
     *
     * who: 这是一个 ID 参数，取决于 which 的值：
     *   如果 which 是 PRIO_PROCESS，who 就是进程 ID（pid）。
     *   如果 which 是 PRIO_PGRP，who 是进程组 ID。
     *   如果 which 是 PRIO_USER，who 是用户 ID（uid）。
     *
     * 特殊值 0 表示当前上下文：
     *   如果 which 是 PRIO_PROCESS，则 who 为 0 表示当前进程。
     *   如果 which 是 PRIO_PGRP，who 为 0 表示当前进程组。
     *   如果 which 是 PRIO_USER，who 为 0 表示当前用户。
     */
    prio = getpriority(PRIO_PROCESS, pid);
    if (prio == -1) {
        perror("getpriority failed");
        return 1;
    }
    printf("Current process priority (nice value): %d\n", prio);

    // 设置当前进程的优先级为 10
    if (setpriority(PRIO_PROCESS, pid, 10) == -1) {
        perror("setpriority failed");
        return 1;
    }

    // 再次获取优先级以验证
    prio = getpriority(PRIO_PROCESS, pid);
    if (prio == -1) {
        perror("getpriority failed");
        return 1;
    }
    printf("New process priority (nice value): %d\n", prio);


    return 0;
}
