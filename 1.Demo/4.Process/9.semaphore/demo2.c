/*************************************************************************
    > File Name: demo2.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Wed 11 Sep 10:34:28 2024
 ************************************************************************/

/*
 * 下面是一个使用C语言实现的进程间信号量通信的例子，使用fork()创建进程，以及使用
 * POSIX信号量（sem_t）和共享内存（shm_open）来在父子进程之间进行通信。
 *
 * 在这个例子中，生产者进程将整数写入共享内存，消费者进程读取这些整数。使用信号量
 * 来同步生产者和消费者，确保消费者只有在生产者生成数据后才能消费。
 *
 * 代码解释：
 * 1. 共享内存：使用shm_open创建共享内存，生产者和消费者通过该内存段进行数据传递。
 *  shm_open：创建或打开一个共享内存对象。
 *  ftruncate：设置共享内存的大小。
 *  mmap：将共享内存映射到进程的地址空间。
 * 2. 信号量：
 *  sem_open：创建或打开一个POSIX信号量。
 *  sem_post：递增信号量，通知其他进程资源可用。
 *  sem_wait：等待信号量，表示进程等待资源。
 * 在本例中，两个信号量分别用于同步生产者和消费者：
 *  sem_producer：表示生产者是否可以生产。
 *  sem_consumer：表示消费者是否可以消费。
 * 3. 进程控制：
 *  使用fork()创建一个子进程，父进程作为生产者，子进程作为消费者。
 *  生产者通过写入共享内存来生成数据，消费者通过读取共享内存来消费数据。
 *  使用信号量来确保同步，防止消费者在生产者没有准备好时读取数据。
 * 4. 资源清理：
 *  父进程等待子进程结束后，关闭并删除信号量和共享内存。
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/wait.h>

#define SHARED_MEM_NAME "/shared_mem"
#define SEM_PRODUCER_NAME "/sem_producer"
#define SEM_CONSUMER_NAME "/sem_consumer"
#define SHARED_MEM_SIZE sizeof(int)

void producer(sem_t *sem_producer, sem_t *sem_consumer, int *shared_mem)
{
    for (int i = 0; i < 5; i++) {
        sleep(rand() % 3);  // 模拟生产者延迟
        int item = rand() % 100;
        *shared_mem = item;  // 将数据写入共享内存
        printf("生产者生产了 %d\n", item);

        sem_post(sem_consumer);  // 通知消费者可以消费
        sem_wait(sem_producer);  // 等待消费者消费
    }
}

void consumer(sem_t *sem_producer, sem_t *sem_consumer, int *shared_mem)
{
    for (int i = 0; i < 5; i++) {
        sem_wait(sem_consumer);  // 等待生产者生产
        int item = *shared_mem;  // 从共享内存中读取数据
        printf("消费者消费了 %d\n", item);

        sem_post(sem_producer);  // 通知生产者已经消费
        sleep(rand() % 3);       // 模拟消费者延迟
    }
}

int main()
{
    // 创建/打开共享内存
    int shm_fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SHARED_MEM_SIZE);
    int *shared_mem = (int *)mmap(0, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // 创建/初始化信号量
    sem_t *sem_producer = sem_open(SEM_PRODUCER_NAME, O_CREAT, 0666, 0);
    sem_t *sem_consumer = sem_open(SEM_CONSUMER_NAME, O_CREAT, 0666, 0);

    pid_t pid = fork();

    if (pid == 0) {
        // 子进程 - 消费者
        consumer(sem_producer, sem_consumer, shared_mem);
        exit(0);
    } else {
        // 父进程 - 生产者
        producer(sem_producer, sem_consumer, shared_mem);
        wait(NULL);  // 等待子进程结束

        // 清理资源
        sem_close(sem_producer);
        sem_close(sem_consumer);
        sem_unlink(SEM_PRODUCER_NAME);
        sem_unlink(SEM_CONSUMER_NAME);
        munmap(shared_mem, SHARED_MEM_SIZE);
        shm_unlink(SHARED_MEM_NAME);
    }

    return 0;
}

