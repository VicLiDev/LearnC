/*************************************************************************
    > File Name: 7.thread_priority.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Thu 10 Oct 2024 08:37:20 PM CST
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>

int main()
{
    // 获取当前进程的优先级
    int current_priority = getpriority(PRIO_PROCESS, 0);
    printf("Current priority: %d\n", current_priority);

    // 设置新的nice值
    int new_priority = 10;  // 新的优先级值
    if (setpriority(PRIO_PROCESS, 0, new_priority) == -1) {
        perror("setpriority failed");
        return 1;
    }

    // 获取并打印更新后的优先级
    int updated_priority = getpriority(PRIO_PROCESS, 0);
    printf("Updated priority: %d\n", updated_priority);

    return 0;
}
