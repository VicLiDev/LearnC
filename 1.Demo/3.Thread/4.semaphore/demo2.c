/*************************************************************************
    > File Name: demo.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Wed 11 Sep 10:40:16 2024
 ************************************************************************/

/*
 * 在这个例子中，生产者线程将数字放入一个缓冲区，而消费者线程从该缓冲区中取走数字。
 * 信号量用于协调两个线程对缓冲区的访问，确保它们不会在同一时间访问或修改缓冲区。
 *
 * 代码说明
 * 1. buffer[]：这是一个缓冲区，大小为5，用于存储生产者产生的项目。
 * 2. in 和 out：in表示生产者写入的下一个位置，out表示消费者读取的下一个位置。
 * 3. 信号量：
 *  empty_slots：表示缓冲区中空闲的槽位数，初始值为缓冲区大小（5）。
 *  filled_slots：表示缓冲区中已填充的槽位数，初始值为0。
 * 4. 互斥锁：用于确保生产者和消费者不会同时访问和修改缓冲区。
 * 5. producer 和 consumer 函数：生产者和消费者分别通过信号量来等待空闲槽和已填充
 *  槽。信号量确保生产者和消费者的行为是同步的，避免资源争用。
 */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>   // for O_CREAT
#include <sys/stat.h> // for mode constants

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];    // 缓冲区
int in = 0;                 // 生产者写入位置
int out = 0;                // 消费者读取位置

sem_t *empty_slots;          // 表示空闲槽的信号量
sem_t *filled_slots;         // 表示已填充槽的信号量
pthread_mutex_t mutex;       // 互斥锁，用于保护缓冲区

// 生产者函数
void* producer(void* arg)
{
    (void)arg;
    int item;

    for (int i = 0; i < 10; i++) {
        item = i; // 生产一个项目

        // 等待空闲槽
        sem_wait(empty_slots);

        // 获取互斥锁以保护缓冲区
        pthread_mutex_lock(&mutex);

        // 将项目添加到缓冲区
        buffer[in] = item;
        printf("Producer produced: %d\n", item);
        in = (in + 1) % BUFFER_SIZE;

        // 释放互斥锁
        pthread_mutex_unlock(&mutex);

        // 增加已填充槽的计数
        sem_post(filled_slots);

        // 模拟生产时间
        sleep(1);
    }
    return NULL;
}

// 消费者函数
void* consumer(void* arg)
{
    (void)arg;
    int item;

    for (int i = 0; i < 10; i++) {
        // 等待已填充槽
        sem_wait(filled_slots);

        // 获取互斥锁以保护缓冲区
        pthread_mutex_lock(&mutex);

        // 从缓冲区中取出项目
        item = buffer[out];
        printf("Consumer consumed: %d\n", item);
        out = (out + 1) % BUFFER_SIZE;

        // 释放互斥锁
        pthread_mutex_unlock(&mutex);

        // 增加空闲槽的计数
        sem_post(empty_slots);

        // 模拟消费时间
        sleep(1);
    }
    return NULL;
}

int main()
{
    pthread_t prod_thread, cons_thread;

    // 创建并初始化信号量
    empty_slots = sem_open("/empty_slots", O_CREAT, 0644, BUFFER_SIZE); // 初始空闲槽为BUFFER_SIZE
    filled_slots = sem_open("/filled_slots", O_CREAT, 0644, 0);          // 初始已填充槽为0

    if (empty_slots == SEM_FAILED || filled_slots == SEM_FAILED) {
        perror("sem_open");
        return -1;
    }

    // 初始化互斥锁
    pthread_mutex_init(&mutex, NULL);

    // 创建生产者和消费者线程
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    // 等待生产者和消费者线程结束
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    // 关闭并删除信号量
    sem_close(empty_slots);
    sem_close(filled_slots);
    sem_unlink("/empty_slots");
    sem_unlink("/filled_slots");

    // 销毁互斥锁
    pthread_mutex_destroy(&mutex);

    return 0;
}

