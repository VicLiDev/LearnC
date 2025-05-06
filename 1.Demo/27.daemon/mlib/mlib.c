/*************************************************************************
    > File Name: mlib.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Mon 05 May 2025 10:48:24 AM CST
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include "mlib.h"

int lib_init()
{
    printf("lib C initialized\n");
    return 0;
}

void lib_do_work(const char *prefix)
{
    printf("==> prefix:%s lib C doing work...\n", prefix);
    // 模拟工作
    /* sleep(1); */
}

void lib_cleanup()
{
    printf("lib C cleanup\n");
}

__attribute__((constructor(101))) void cons_init()
{
    printf("lib C func:%s\n", __func__);
}

__attribute__((constructor(102))) void cons_init2()
{
    printf("lib C func:%s\n", __func__);
}

__attribute__((destructor(102))) void dest_deinit2()
{
    printf("lib C func:%s\n", __func__);
}

__attribute__((destructor(101))) void dest_deinit()
{
    printf("lib C func:%s\n", __func__);
}
