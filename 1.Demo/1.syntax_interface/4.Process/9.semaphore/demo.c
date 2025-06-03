/*************************************************************************
    > File Name: demo.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Wed 11 Sep 10:31:20 2024
 ************************************************************************/

/* 在进程间通信中，信号量通常用于同步共享资源的访问，但信号量本身并不能传递数据。
 * 如果只使用信号量来实现同步，而不传递数据，可以通过信号量来控制两个进程的执行
 * 顺序，比如让一个生产者等待消费者，反之亦然。这种模式依赖于信号量的互斥机制，
 * 而不涉及实际的数据传递。
 *
 * 代码解释：
 * 1. 信号量的创建和初始化：
 *   使用sem_open()来创建或打开命名信号量。
 *   sem_producer信号量用于控制生产者，初始值为0，表示生产者需要等待消费者的信号
 *   才能继续生产。
 *   sem_consumer信号量用于控制消费者，初始值为0，表示消费者需要等待生产者生产
 *   完成后才能消费。
 * 2. 生产者逻辑：
 *  生产者生产数据时，使用sem_post(sem_consumer)通知消费者它可以消费了。
 *  然后，生产者调用sem_wait(sem_producer)等待消费者处理完数据，确保在消费者消费
 *  完之前不会继续生产。
 * 3. 消费者逻辑：
 *  消费者首先调用sem_wait(sem_consumer)等待生产者生产完成。
 *  消费数据后，调用sem_post(sem_producer)通知生产者继续生产。
 * 4. 进程管理：
 *  使用fork()创建子进程，父进程作为生产者，子进程作为消费者。
 *  父进程等待子进程结束，然后清理信号量。
 * 5. 信号量的清理：
 *  使用sem_close()关闭信号量。
 *  使用sem_unlink()删除命名信号量。
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>  // For O_CREAT, O_EXCL
#include <sys/wait.h>

// 定义信号量的名称
#define SEM_PRODUCER_NAME "/sem_producer"
#define SEM_CONSUMER_NAME "/sem_consumer"

void producer(sem_t *sem_producer, sem_t *sem_consumer)
{
    for (int i = 0; i < 5; i++) {
        printf("生产者：生产第 %d 个产品\n", i+1);
        sleep(1);  // 模拟生产时间

        // 生产完一个产品后，通知消费者可以消费
        sem_post(sem_consumer);

        // 等待消费者消费完后，再继续生产
        sem_wait(sem_producer);
    }
}

void consumer(sem_t *sem_producer, sem_t *sem_consumer)
{
    for (int i = 0; i < 5; i++) {
        // 等待生产者生产产品
        sem_wait(sem_consumer);

        printf("消费者：消费第 %d 个产品\n", i+1);
        sleep(1);  // 模拟消费时间

        // 消费完后通知生产者可以继续生产
        sem_post(sem_producer);
    }
}

int main()
{
    // 创建/打开信号量
    sem_t *sem_producer = sem_open(SEM_PRODUCER_NAME, O_CREAT, 0666, 0);
    sem_t *sem_consumer = sem_open(SEM_CONSUMER_NAME, O_CREAT, 0666, 0);

    if (sem_producer == SEM_FAILED || sem_consumer == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // 子进程 - 消费者
        consumer(sem_producer, sem_consumer);
        exit(0);
    } else {
        // 父进程 - 生产者
        producer(sem_producer, sem_consumer);
        wait(NULL);  // 等待子进程结束

        // 清理信号量
        sem_close(sem_producer);
        sem_close(sem_consumer);
        sem_unlink(SEM_PRODUCER_NAME);
        sem_unlink(SEM_CONSUMER_NAME);
    }

    return 0;
}

