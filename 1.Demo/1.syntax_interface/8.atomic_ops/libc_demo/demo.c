/*************************************************************************
    > File Name: demo.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Wed 25 Jun 2025 03:47:31 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define THREAD_COUNT 4

atomic_int counter = 0;             // 原子计数器
atomic_bool flag = false;           // 原子布尔值
atomic_intptr_t shared_ptr = 0;     // 原子指针（整型模拟指针地址）

void* increment_counter(void* arg)
{
    (void)arg;
    for (int i = 0; i < 1000; i++) {
        atomic_fetch_add(&counter, 1);  // 原子加法
    }
    return NULL;
}

void* set_flag_once(void* arg)
{
    (void)arg;
    bool expected = false;
    if (atomic_compare_exchange_strong(&flag, &expected, true)) {
        printf("Thread %ld set the flag to true.\n", pthread_self());
    } else {
        printf("Thread %ld saw flag already set.\n", pthread_self());
    }
    return NULL;
}

void* exchange_pointer(void* arg)
{
    intptr_t old_ptr = atomic_exchange(&shared_ptr, (intptr_t)arg);
    printf("Thread %ld changed ptr from %ld to %ld\n", pthread_self(), old_ptr, (intptr_t)arg);
    return NULL;
}

int main()
{
    pthread_t threads[THREAD_COUNT];

    // 启动计数器线程
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, increment_counter, NULL);
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final counter value: %d\n", atomic_load(&counter));

    // 启动 flag 设置线程（只有一个会成功设置）
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, set_flag_once, NULL);
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    // 启动指针交换线程
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, exchange_pointer, (void*)(intptr_t)(i + 100));
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

