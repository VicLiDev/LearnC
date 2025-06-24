/*************************************************************************
    > File Name: main.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 24 Jun 2025 06:08:27 PM CST
 ************************************************************************/

#include "mem_pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define THREADS 4
#define ALLOCS  1000

void* thread_func(void* arg)
{
    for (int i = 0; i < ALLOCS; ++i) {
        size_t sz = rand() % 1024 + 1;
        void* p = mem_pool_alloc_label(sz, "worker");
        usleep(1000);
        mem_pool_free(p);
    }
    return NULL;
}

int main()
{
    mem_pool_init();
    mem_pool_set_debug(1);
    mem_pool_set_protection(1);

    pthread_t th[THREADS];
    for (int i = 0; i < THREADS; ++i)
        pthread_create(&th[i], NULL, thread_func, NULL);

    for (int i = 0; i < THREADS; ++i)
        pthread_join(th[i], NULL);


    /* void* leak = mem_pool_alloc_label(1024, "leak test"); */

    mem_pool_dump_stats();
    mem_pool_dump_leaks();
    mem_pool_destroy();
    return 0;
}
