/*
 * 代码说明：
 * 1. ftok("shmfile", 65)：生成一个用于标识共享内存的键值。这个函数通过文件路径
 *    和项目ID生成一个唯一的键。
 * 2. shmget()：创建或获取一个共享内存段，大小为 SHM_SIZE。
 * 3. shmat()：将共享内存段附加到当前进程的地址空间，返回共享内存段的指针。
 * 4. 父进程写入数据，子进程读取数据。
 * 5. shmdt()：在父进程和子进程完成操作后，将共享内存段从各自的地址空间中分离。
 * 6. shmctl(shmid, IPC_RMID, NULL)：删除共享内存段，释放资源。
 *
 * 父进程写入共享内存，子进程读取共享内存，但两者的执行顺序并没有严格的控制。如果
 * 父进程还没来得及写入数据，而子进程已经开始读取共享内存，就可能会导致竞态条件，
 * 导致子进程读取到的内容不完整或无效。因此这里给的demo使用了信号量同步机制，确保
 * 子进程读取共享内存时，父进程已经完成写入。
 *
 * 同步说明：
 * 1. 信号量：通过 semget() 创建一个信号量，并使用 semop() 函数来进行信号量操作。
 *    我们使用信号量的两个常见操作：
 *    sem_wait()：子进程在读共享内存之前等待信号量的值变为正数。
 *    sem_signal()：父进程写入完成后，将信号量值加 1，通知子进程可以读取。
 * 2. 同步机制：在父进程写入数据后，通过 sem_signal() 通知子进程数据已写入，子
 *    进程在 sem_wait() 后才读取共享内存中的数据。这种机制确保了子进程不会在父
 *    进程写入完成前开始读取，避免了竞态条件。
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/wait.h>

// 定义共享内存的大小
#define SHM_SIZE 1024

int old_demo();


// 信号量操作：等待 P 操作
void sem_wait(int semid) {
    struct sembuf sb = {0, -1, 0};  // 将信号量的值减 1
    semop(semid, &sb, 1);
}

// 信号量操作：通知 V 操作
void sem_signal(int semid) {
    struct sembuf sb = {0, 1, 0};   // 将信号量的值加 1
    semop(semid, &sb, 1);
}


int main()
{

    // 创建信号量
    int semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (semid == -1) {
        perror("semget failed");
        exit(1);
    }

    // 初始化信号量的值为 0（子进程需要等待父进程写入）
    semctl(semid, 0, SETVAL, 0);

    // 生成一个唯一的键值，用于标识共享内存段
    key_t key = ftok("shmfile", 65);

    // 创建一个共享内存段
    int shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    // 创建一个子进程
    pid_t pid = fork();

    if (pid < 0) {
        // 创建进程失败
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // 子进程：读取共享内存中的数据
        // 将共享内存段附加到子进程的地址空间
        char *shared_memory = (char *)shmat(shmid, NULL, 0);
        if (shared_memory == (char *)-1) {
            perror("shmat failed in child");
            exit(1);
        }

        // 等待父进程写入数据（信号量 P 操作）
        sem_wait(semid);

        // 读取共享内存
        printf("子进程从共享内存读取到的数据: %s\n", shared_memory);

        // 分离共享内存
        if (shmdt(shared_memory) == -1) {
            perror("shmdt failed in child");
            exit(1);
        }

    } else {
        // 父进程：写入数据到共享内存
        // 将共享内存段附加到父进程的地址空间
        char *shared_memory = (char *)shmat(shmid, NULL, 0);
        if (shared_memory == (char *)-1) {
            perror("shmat failed in parent");
            exit(1);
        }

        // 向共享内存写入数据
        char *message = "Hello from parent!";
        strncpy(shared_memory, message, SHM_SIZE);
        printf("父进程已将数据写入共享内存: %s\n", message);

        // 写入完成，通知子进程可以读取（信号量 V 操作）
        sem_signal(semid);

        // 等待子进程执行完毕
        wait(NULL);

        // 分离共享内存
        if (shmdt(shared_memory) == -1) {
            perror("shmdt failed in parent");
            exit(1);
        }

        // 删除共享内存段
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl failed");
            exit(1);
        }

        // 删除信号量
        semctl(semid, 0, IPC_RMID);

        // 旧的demo
        old_demo();
    }

    return 0;
}


int old_demo()
{
    int segment_id;
    char* shared_memory;
    struct shmid_ds shmbuffer;
    int segment_size;
    const int shared_segment_size = 0x6400;
    /* 分配一个共享内存块 */
    segment_id = shmget(IPC_PRIVATE, shared_segment_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR );
    /* 绑定到共享内存块 */
    shared_memory = (char*) shmat (segment_id, 0, 0);
    printf("shared memory attached at address %p\n", shared_memory);
    /* 确定共享内存的大小 */
    shmctl (segment_id, IPC_STAT, &shmbuffer);
    segment_size = shmbuffer.shm_segsz;
    printf ("segment size: %d\n", segment_size);
    /* 在共享内存中写入一个字符串 */
    sprintf (shared_memory, "Hello, world.");
    /* 脱离该共享内存块 */
    shmdt (shared_memory);
    /* 重新绑定该内存块 */
    shared_memory = (char*) shmat (segment_id, (void*) 0x500000, 0);
    printf("shared memory reattached at address %p\n", shared_memory);
    /* 输出共享内存中的字符串 */
    printf ("%s\n", shared_memory);
    /* 脱离该共享内存块 */
    shmdt (shared_memory);
    /* 释放这个共享内存块 */
    shmctl (segment_id, IPC_RMID, 0);
    return 0;
}
