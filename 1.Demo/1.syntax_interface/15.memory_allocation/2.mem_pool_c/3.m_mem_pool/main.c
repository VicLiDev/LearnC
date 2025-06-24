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

void* thread_func(void *arg)
{
    mem_pool_ctx *mp_ctx = (mem_pool_ctx *)arg;

    for (int i = 0; i < ALLOCS; ++i) {
        size_t sz = rand() % 1024 + 1;
        void* p = mem_pool_alloc_label(mp_ctx, sz, "worker");
        usleep(1000);
        mem_pool_free(mp_ctx, p);
    }
    return NULL;
}

int main()
{
    mem_pool_ctx *mp_ctx;

    mem_pool_init(&mp_ctx);
    mem_pool_set_debug(mp_ctx, 1);
    mem_pool_set_protection(mp_ctx, 1);

    pthread_t th[THREADS];
    for (int i = 0; i < THREADS; ++i)
        pthread_create(&th[i], NULL, thread_func, mp_ctx);

    for (int i = 0; i < THREADS; ++i)
        pthread_join(th[i], NULL);


    /* void *leak = mem_pool_alloc_label(mp_ctx, 1024, "leak test"); */

    mem_pool_dump_stats(mp_ctx);
    mem_pool_dump_leaks(mp_ctx);
    mem_pool_destroy(mp_ctx);
    return 0;
}
