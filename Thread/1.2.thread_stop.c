// 如果需要只终止某个线程而不是整个进程都终止，有三种方法。
// 1). 从线程函数return，对主线程不使用，在main函数中return 相当于exit。
// 2). 一个线程可以调用pthread_cancel() 终止同一进程中的另一个线程。比较复杂，暂不分析。
// 3). 线程可以调用 pthread_exit() 终止自己。

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


// 终止线程 方法1

void * run1(void * arg)
{
    printf("I am tread_1, I am still alive, after a second I will quit.\n");
    sleep(1);

    return NULL;

    printf("Never output.\n");
}

// 终止线程 方法2

void * run2(void *arg)
{
    sleep(1);
    printf("I am tread_2, I am still alive, I will quit.\n");
    pthread_exit(NULL);
    printf("never output .\n");
}

int main()
{
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, run1, NULL);
    pthread_create(&tid2, NULL, run2, NULL);

    sleep(2); // 确保主线程最后退出 
    printf("The thread quit, I should quit.\n");
    return 0;
}