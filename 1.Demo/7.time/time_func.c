/*************************************************************************
    > File Name: time_func.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Thu 10 Oct 2024 04:50:57 PM CST
 ************************************************************************/

/*
 * 1秒(s) = 1,000毫秒(ms)
 * 1毫秒(ms) = 1,000微秒(us)
 * 1微秒(us) = 1,000纳秒(ns)
 *
 * 说明：
 * 1. get_local_time() 使用 time() 和 localtime() 获取当前本地时间并输出。
 * 2. get_high_precision_time() 使用 clock_gettime() 获取高精度时间，精确到纳秒。
 * 3. format_time() 使用 strftime() 将当前时间格式化为可读的字符串形式。
 * 4. measure_cpu_time() 使用 clock() 计算程序的运行时间，用于测量程序的耗时。
 *
 *
 * 函数"clock_gettime"是基于Linux C语言的时间函数,可以用于计算时间，有秒和纳秒两种精度。
 * 函数原型：
 * int clock_gettime(clockid_t clk_id, struct timespec *tp);
 * 其中，cld_id类型四种：
 *   a、CLOCK_REALTIME:系统实时时间,随系统实时时间改变而改变,即从UTC1970-1-1 0:0:0
 *      开始计时,中间时刻如果系统时间被用户改成其他,则对应的时间相应改变。
 *   b、CLOCK_MONOTONIC:从系统启动这一刻起开始计时,不受系统时间被用户改变的影响
 *   c、CLOCK_PROCESS_CPUTIME_ID:本进程到当前代码系统CPU花费的时间
 *   d、CLOCK_THREAD_CPUTIME_ID:本线程到当前代码系统CPU花费的时间
 *
 * 本文默认采用CLOCK_REALTIME，即可实现并行程序的准确计时。
 * 其中，timespec结构包括：
 *   struct timespec {
 *      time_t tv_sec; // 秒
 *      long tv_nsec; // 纳秒
 *  };
 *
 *
 * int gettimeofday(struct timeval *tv,struct timezone *tz);
 * strut timeval {
 *     long tv_sec; // 秒数
 *     long tv_usec; // 微秒数
 * };
 * 
 * gettimeofday将时间保存在结构tv之中.tz一般我们使用NULL来代替.
 */

#include <stdio.h>
#include <time.h>
#include <sys/time.h>   // gettimeofday 接口使用

// 使用 time() 和 localtime() 获取本地时间
void get_local_time()
{
    time_t current_time;
    struct tm *local_time;
    char *timeString = NULL;

    // 获取当前时间
    // time(&current_time) 直接将结果保存在传入的指针指向的变量中，适合直接修改
    // 已存在的变量。
    // time(NULL) 返回值形式更简洁，不使用指针，直接将结果赋值给一个新变量。适合
    // 不关心指针的情况下使用。
    time(&current_time);
    // current_time = time(NULL);

    // 转换为本地时间
    local_time = localtime(&current_time);

    // 输出时间
    printf("当前本地时间: %02d-%02d-%04d %02d:%02d:%02d\n",
           local_time->tm_mday,
           local_time->tm_mon + 1,
           local_time->tm_year + 1900,
           local_time->tm_hour,
           local_time->tm_min,
           local_time->tm_sec);

    // 将time_t类型转换为字符串形式
    timeString = ctime(&current_time);
    // 打印当前时间
    printf("当前系统时间： %s", timeString);
}

// 使用 clock_gettime() 获取高精度时间（精确到纳秒）
void get_high_precision_time()
{
    struct timespec ts;

    // 获取高精度时间
    clock_gettime(CLOCK_REALTIME, &ts);

    // 输出秒和纳秒
    printf("秒: %ld\n", ts.tv_sec);
    printf("纳秒: %ld\n", ts.tv_nsec);
}

// 使用 strftime() 格式化时间
void format_time()
{
    time_t current_time;
    struct tm *local_time;
    char time_str[100];

    // 获取当前时间
    time(&current_time);
    local_time = localtime(&current_time);

    // 使用 strftime 格式化时间
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);

    printf("格式化时间: %s\n", time_str);
}

// 使用 clock() 计算程序运行时间
void measure_cpu_time()
{
    clock_t start, end;
    double cpu_time_used;

    // 获取开始时间
    start = clock();

    // 模拟一些计算任务
    for (volatile long i = 0; i < 100000000; i++);

    // 获取结束时间
    end = clock();

    // 计算CPU使用时间
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("程序运行时间: %f 秒\n", cpu_time_used);
}

// 使用 clock_gettime() 计算程序运行时间
void measure_cpu_time2()
{
    struct timespec time1 = {0, 0};
    struct timespec time2 = {0, 0};

    clock_gettime(CLOCK_REALTIME, &time1);
    {
        unsigned int i,j;
        unsigned int y = 0;
        (void)y;
        for(i=0; i<1000; i++)
            for(j=0; j<1000; j++)
                y *= i;
    }
    clock_gettime(CLOCK_REALTIME, &time2);

    printf("time passed is: %ld ms\n",
            (time2.tv_sec - time1.tv_sec)*1000
            + (time2.tv_nsec - time1.tv_nsec)/1000000);
}

// 使用 gettimeofday() 计算程序运行时间
void measure_cpu_time3()
{
    struct timeval tmstart,tmend;
    long timeuse;

    gettimeofday(&tmstart,NULL);
    {
        unsigned int i,j;
        unsigned int y = 0;

        (void)y;
        for(i=0; i<1000; i++)
            for(j=0; j<1000; j++)
                y *= i;
    }
    gettimeofday(&tmend,NULL);

    timeuse = 1000000*(tmend.tv_sec-tmstart.tv_sec) + (tmend.tv_usec-tmstart.tv_usec);
    timeuse /= 1000;
    printf("Used Time:%ld ms\n",timeuse);
}

int main(int argc, char *argv[], char *envp[])
{
    (void)argc;
    (void)argv;
    (void)envp;

    printf("=== 获取本地时间 (localtime) ===\n");
    get_local_time();

    printf("\n=== 获取高精度时间 (clock_gettime) ===\n");
    get_high_precision_time();

    printf("\n=== 格式化时间输出 (strftime) ===\n");
    format_time();

    printf("\n=== 计算CPU运行时间 (clock) ===\n");
    measure_cpu_time();
    measure_cpu_time2();
    measure_cpu_time3();

    return 0;
}
