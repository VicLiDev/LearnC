/*************************************************************************
    > File Name: 25.atomic_ops_demo.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Fri 27 Dec 2024 05:48:21 PM CST
 ************************************************************************/

/*
 * 示例：GCC 内建原子操作函数演示
 *
 * 包含以下原子操作：
 * 1. 加法和减法操作
 * 2. 按位操作
 * 3. 比较并交换操作 (CAS)
 * 4. 自旋锁操作
 * 5. 内存屏障
 *
 * 注意：
 * 1. __sync_* 系列函数适合简单场景，但控制粒度较低。
 * 2. 对于现代开发，应优先考虑 C++11 的 <atomic> 或 __atomic_* 系列函数。
 */

#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

// 定义共享变量
volatile int counter = 0;
volatile int lock = 0; // 自旋锁

// 线程函数：原子加法和减法操作
void* thread_func_arithmetic(void* arg)
{
    (void)arg;

    for (int i = 0; i < 10000; ++i) {
        // 原子加法和减法操作
        __sync_fetch_and_add(&counter, 1); // 加 1，返回旧值
        __sync_fetch_and_sub(&counter, 1); // 减 1，返回旧值
        __sync_add_and_fetch(&counter, 1);
        __sync_sub_and_fetch(&counter, 1);
    }
    return NULL;
}

// 原子按位操作示例
void bit_operations_demo()
{
    int value = 0b1100; // 初始值为 12 (二进制 1100)

    // 执行按位操作
    __sync_fetch_and_or(&value, 0b0011);  // 按位 OR 操作
    __sync_fetch_and_and(&value, 0b1110); // 按位 AND 操作
    __sync_fetch_and_xor(&value, 0b0101); // 按位 XOR 操作

    printf("After bit operations: %d\n", value);
}

// 原子比较并交换操作示例
void cas_demo()
{
    int old_val = 0;
    int new_val = 1;

    // 如果 counter == old_val，则将其设置为 new_val
    bool success = __sync_bool_compare_and_swap(&counter, old_val, new_val);
    if (success) {
        printf("CAS Success: counter = %d\n", counter);
    } else {
        printf("CAS Failed: counter = %d\n", counter);
    }
}

// 自旋锁示例
void spinlock_demo()
{
    // 获取锁
    while (__sync_lock_test_and_set(&lock, 1)) {
        // 自旋等待锁释放
    }
    printf("Lock acquired!\n");

    // 执行临界区代码
    counter += 1;

    // 释放锁
    __sync_lock_release(&lock);
    printf("Lock released!\n");
}

// 内存屏障示例
void memory_barrier_demo()
{
    int a = 0;
    int b = 0;

    __sync_synchronize(); // 确保所有内存操作的顺序不会被重新排列

    a = 1;
    __sync_synchronize(); // 插入屏障，确保 a 的赋值操作先执行

    b = 2;
    printf("Memory barrier example: a=%d, b=%d\n", a, b);
}

int main()
{
    pthread_t t1, t2;

    // 启动线程进行原子加减操作
    pthread_create(&t1, NULL, thread_func_arithmetic, NULL);
    pthread_create(&t2, NULL, thread_func_arithmetic, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final counter value: %d\n", counter);

    // 按位操作
    bit_operations_demo();

    // CAS 操作
    cas_demo();

    // 自旋锁操作
    spinlock_demo();

    // 内存屏障
    memory_barrier_demo();

    return 0;
}
