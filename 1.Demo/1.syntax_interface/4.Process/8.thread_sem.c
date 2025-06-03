/*
 * =============================================================================
 * 分类：
 * 内核信号量
 * 用户信号量--+--POSIX信号量--+--有名信号量
 *             |               +--无名信号量
 *             |
 *             +--SYSTEMV信号量
 *
 * ---- 内核信号量 ----
 * #include
 * void sema_init(struct semaphore *sem, int val);
 * void init_MUTEX(struct semaphore *sem); //初始值1
 * void init_MUTEX_LOCKED(struct semaphore *sem); //初始值0
 * void down(struct semaphore *sem); //可睡眠
 * int down_interruptible(struct semaphore *sem); //可中断
 * int down_trylock(struct semaphore *sem); //m非阻塞
 * void up(struct semaphore *sem);
 *
 * ---- SYSTEMV信号量 ----
 * #include
 * int semget(key_t key, int nsems, int oflag);
 * int semop(int semid, struct sembuf *opsptr, size_t nops);
 * int semctl(int semid, int semum, int cmd,...);
 *
 * ---- POSIX无名信号量 ----
 * #include
 * sem_t sem;
 * //pshared为0则线程间共享，pshared为1则父子进程共享
 * int sem_init(sem_t *sem, int pshared, unsigned int val);
 * int sem_wait(sem_t *sem); //阻塞
 * int sem_trywait(sem_t *sem); //非阻塞
 * int sem_post(sem_t *sem);
 * int sem_destroy(sem_t *sem);
 * 进程间共享则sem必须放在共享内存区域（mmap, shm_open, shmget），父进程的全局
 * 变量、堆、栈中存储是不行的
 *
 * ---- POSIX有名信号量 ----
 * sem_t *sem_open(const char *name, int oflag, mode_t mode, int val);
 * int sem_trywait(sem_t *sem);
 * int sem_post(sem_t *sem);
 * int sem_close(sem_t *sem);
 * int sem_unlink(const char *name);
 * 每个open的位置都要close和unlink，但只有最后执行的unlink生效
 */

/*
 * =============================================================================
 *
 * 这里重点描述了POSIX标准下的信号量使用。
 * 信号量本质上是一个计数器，表征资源可用数量。大于0时资源可访问，小于等于0时，
 * 资源不可访问，线程只能等待；
 *
 * sem_t : 定义在semaphore.h头文件中
 *
 * sem_t相关的操作如下：
 * int sem_init(sem_t* __sem, int __shared, unsigned int __value);
 * int sem_wait(sem_t* __sem);
 * int sem_post(sem_t* __sem);
 * int sem_getvalue(sem_t* __sem, int* __value);
 * int sem_timedwait(sem_t* __sem, const struct timespec* __ts);
 * int sem_trywait(sem_t* __sem);
 * int sem_destroy(sem_t* __sem);
 *
 * int sem_init(sem_t* __sem, int __shared, unsigned int __value);
 * 初始化信号量，包含3个参数，分别是信号量指针，共享模式，value初始值，分别解释下：
 * __sem: 要初始化的信号量指针
 * __shared: 若为0，表示进程内私有，只能进程内多线程共享，若不为0，表示可以进程间
 *           共享（有说法不为0可能造成函数调用失败，属于无名信号量，待验证）
 * __value: 信号量的初值
 *
 * int sem_wait(sem_t* __sem);
 * 阻塞当前线程，直到信号量的值大于0，接触阻塞后将信号量的值减1，表示公共资源使用
 * 后减少；
 * 是一个原子操作；
 *
 * int sem_post(sem_t* __sem);
 * 该线程会释放资源，使信号量的值加1；当有某个线程等待这个信号量时，该线程即可以
 * 运行，并且将将信号量的值减1（参见sem_wait）；
 *
 * int sem_destroy(sem_t* __sem);
 * 销毁信号量
 */

/*
 * POSIX 信号量与 System V 信号量的比较：
 * 1. 接口和使用：POSIX 信号量更现代，API 简单，可以在进程或线程之间使用。
 * 2. 命名信号量：POSIX 支持命名信号量（跨进程），System V 信号量使用键值生成标识。
 * 3. 移植性：POSIX 信号量通常在更广泛的系统上可用，尤其是在 POSIX 兼容系统
 *    （如 Linux、macOS 等）中。
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

/*
 * system V
 *
 * 代码说明：
 * 1. semget()：创建一个信号量集，使用 ftok() 函数生成唯一的键。信号量集包含一个
 *    信号量。
 * 2. semctl()：用于初始化和删除信号量。我们将信号量的初始值设置为 0，表示子进程
 *    需要等待。
 * 3. semop()：用来执行信号量的操作，sem_wait() 是 P 操作（等待操作），
 *    sem_signal() 是 V 操作（释放操作）。
 * 4. 父进程：模拟了一些工作，完成后通过 sem_signal() 释放信号量，通知子进程继续
 *    执行。
 * 5. 子进程：在 sem_wait() 阻塞等待信号量，直到父进程释放信号量后，子进程才继续
 *    执行。
 *
 * 信号量的工作原理：
 * 1. P 操作（sem_wait）：如果信号量的值为 0，进程将阻塞等待；如果信号量的值大于 0，
 *    信号量值减 1，进程继续执行。
 * 2. V 操作（sem_signal）：将信号量的值加 1，唤醒一个等待该信号量的进程。
 */

// 信号量操作：P操作（等待操作）
void m_sem_wait(int semid)
{
    struct sembuf sb = {0, -1, 0};  // 将信号量的值减 1
    semop(semid, &sb, 1);
}

// 信号量操作：V操作（释放操作）
void m_sem_signal(int semid)
{
    struct sembuf sb = {0, 1, 0};   // 将信号量的值加 1
    semop(semid, &sb, 1);
}

int system_v()
{
    // 创建一个唯一的键值用于信号量
    key_t key = ftok("semfile", 65);

    // 创建信号量集，包含一个信号量，权限设置为 0666
    int semid = semget(key, 1, 0666 | IPC_CREAT);
    if (semid == -1) {
        perror("semget failed");
        exit(1);
    }

    // 初始化信号量的值为 0，子进程会等待父进程通知
    semctl(semid, 0, SETVAL, 0);

    // 创建一个子进程
    pid_t pid = fork();

    if (pid < 0) {
        // fork 失败
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // 子进程：等待父进程的信号量释放
        printf("子进程：等待父进程完成写入...\n");

        // 等待父进程释放信号量（P操作）
        m_sem_wait(semid);

        // 当信号量释放后，子进程继续执行
        printf("子进程：收到信号量通知，可以继续执行了。\n");

    } else {
        // 父进程：写入数据，然后通知子进程
        printf("父进程：正在执行任务...\n");
        sleep(2);  // 模拟父进程的工作

        // 任务完成后，释放信号量，通知子进程
        printf("父进程：任务完成，通知子进程。\n");
        m_sem_signal(semid);

        // 等待子进程完成
        wait(NULL);

        // 删除信号量集
        semctl(semid, 0, IPC_RMID);

        printf("父进程：已删除信号量。\n");
    }

    return 0;
}

/*
 * POSIX
 *
 * 代码说明：
 * 1. sem_open()：创建或打开一个命名的 POSIX 信号量。这里使用的信号量名字是
 *    "/posix_sem"。O_CREAT 表示如果信号量不存在则创建，初始值为 0，表示子进程
 *    需要等待父进程的通知。
 * 2. sem_wait()：子进程在这里等待父进程释放信号量。如果信号量的值为 0，子进程会
 *    阻塞，直到父进程调用 sem_post()。
 * 3. sem_post()：父进程执行完任务后，调用 sem_post() 来释放信号量，通知子进程
 *    继续执行。
 * 4. sem_close() 和 sem_unlink()：关闭并删除信号量。sem_unlink() 删除信号量的
 *    名字，释放系统资源。
 *
 * POSIX 信号量的工作原理：
 * 1. sem_wait()：如果信号量的值为 0，调用进程将阻塞，直到信号量值大于 0；如果
 *    信号量值大于 0，信号量的值减 1，调用进程继续执行。
 * 2. sem_post()：将信号量的值加 1，唤醒等待该信号量的进程（如有）。
 */


#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>      // For O_CREAT
#include <sys/stat.h>   // For mode constants
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int posix()
{
    // 创建或打开一个命名信号量
    sem_t *sem = sem_open("/posix_sem", O_CREAT, 0644, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }

    // 创建一个子进程
    pid_t pid = fork();

    if (pid < 0) {
        // fork 失败
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // 子进程：等待父进程的信号量通知
        printf("子进程：等待父进程完成...\n");

        // 等待信号量被释放
        sem_wait(sem);

        // 当信号量释放后，子进程继续执行
        printf("子进程：收到信号量通知，继续执行。\n");

        // 子进程结束
    } else {
        // 父进程：执行一些操作，然后通知子进程
        printf("父进程：正在执行任务...\n");
        sleep(2);  // 模拟父进程的工作

        // 完成任务后，释放信号量，通知子进程
        printf("父进程：任务完成，通知子进程。\n");
        sem_post(sem);  // V操作，释放信号量

        // 等待子进程完成
        wait(NULL);

        // 关闭和删除信号量
        sem_close(sem);
        sem_unlink("/posix_sem");

        printf("父进程：已删除信号量。\n");
    }

    return 0;
}


int main()
{

    printf("======> lhj add file:%s func:%s line:%d system V\n",
            __FILE__, __func__, __LINE__);
    system_v();

    printf("======> lhj add file:%s func:%s line:%d posix\n",
            __FILE__, __func__, __LINE__);
    posix();
}
