/*************************************************************************
    > File Name: main.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 24 Jun 2025 06:08:27 PM CST
 ************************************************************************/

#include "mem_pool.h"
#include <stdio.h>
#include <string.h>

int main()
{
    mem_pool_init();

    char* a = (char*)mem_pool_alloc(100);
    strcpy(a, "hello world");
    printf("A: %s\n", a);

    int* b = (int*)mem_pool_alloc(2048);
    for (int i = 0; i < 10; ++i) b[i] = i * i;

    char* leak = (char*)mem_pool_alloc(200); // 故意泄漏

    mem_pool_free(a);
    mem_pool_free(b);

    mem_pool_destroy(); // 自动 dump stats + leaks

    return 0;
}

