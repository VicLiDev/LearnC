/*************************************************************************
    > File Name: 22.cpu_mem_query.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 15 Oct 2024 10:05:42 AM CST
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void get_cpu_times(unsigned long long int *idle, unsigned long long int *total)
{
    FILE *fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        perror("fopen");
        return;
    }

    char buffer[1024];
    fgets(buffer, sizeof(buffer), fp);

    unsigned long long int user, nice, system, idle_time, iowait, irq, softirq, steal;
    sscanf(buffer, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu",
           &user, &nice, &system, &idle_time, &iowait, &irq, &softirq, &steal);

    *idle = idle_time + iowait;
    *total = user + nice + system + idle_time + iowait + irq + softirq + steal;

    fclose(fp);
}

void get_memory_usage(long *total, long *free)
{
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        perror("fopen");
        return;
    }

    char buffer[256];
    fgets(buffer, sizeof(buffer), fp);
    sscanf(buffer, "MemTotal: %ld kB", total);

    fgets(buffer, sizeof(buffer), fp);
    sscanf(buffer, "MemFree: %ld kB", free);

    fclose(fp);
}

int main()
{
    unsigned long long int idle1, total1;
    unsigned long long int idle2, total2;
    long mem_total, mem_free;

    // 获取第一次 CPU 时间
    get_cpu_times(&idle1, &total1);

    // 等待一秒钟
    sleep(1);

    // 获取第二次 CPU 时间
    get_cpu_times(&idle2, &total2);

    // 计算 CPU 占用率
    unsigned long long int idle_diff = idle2 - idle1;
    unsigned long long int total_diff = total2 - total1;
    double cpu_usage = 100.0 * (1.0 - (double)idle_diff / total_diff);

    // 获取内存使用情况
    get_memory_usage(&mem_total, &mem_free);
    long mem_used = mem_total - mem_free;
    double mem_usage = 100.0 * (double)mem_used / mem_total;

    // 输出 CPU 和内存占用率
    printf("CPU usage: %.2f%%\n", cpu_usage);
    printf("Memory usage: %.2f%% (Used: %ld kB, Free: %ld kB)\n",
           mem_usage, mem_used, mem_free);

    return 0;
}

