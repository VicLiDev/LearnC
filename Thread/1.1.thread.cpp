//使用 g++ 编译需要增加 -lpthread 库
//一篇很不错的博客: https://blog.csdn.net/a1414345/article/details/70947396
//线程主要有三种状态分别是就绪、阻塞、运行。
//就绪：线程具备运行的所有条件，逻辑上已可以运行，在等待处理机。
//阻塞：指线程在等待某一事件的发生，如I/O操作。
//运行：占有处理器正在运行。

#include <stdio.h>
#include <pthread.h>    // pthread_create()
#include <unistd.h>     // sleep()
#include <sys/types.h>  // getpid()

// 打印每个线程的ID， 和进行ID

void * run_1(void *arg)  // 线程1 执行代码
{
    sleep(1);
    printf(" thread 1 tid is  %u,  pid is %u \n", pthread_self(), getpid()); 
}

void * run_2(void *arg)  // 线程2 执行代码
{

    sleep(1);
    printf(" thread 2 tid is  %u,  pid is %u \n", pthread_self(), getpid());
}

int main()
{
    pthread_t tid1, tid2;

    pthread_create(&tid1, NULL, run_1, NULL ); // 创建线程1
    pthread_create(&tid2, NULL, run_2, NULL ); // 创建线程2

    sleep(2);  //由于各个线程独立运行,但主线程结束之后子线程也均会结束,为了避免在子线程运行结束之前主线程结束,所以增加主线程延时
    printf("I am main tid is  %u,  pid is %u \n", pthread_self(), getpid());
    return 0;
}
