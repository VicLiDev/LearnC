// 说说我理解的为什么需要线程等待，有时候需要让一个线程去执行一段代码，我们需要知道它是否帮我们完成了指定的要求，或者异常终止，这时候我们就需要获取线程运行结果，线程退出可以通过返回值带出或者通过pthread_exit()参数带出，拿到它的“遗言”。线程等待也有回收资源的用处，如果一个线程结束运行但没有被等待，那么它类似于僵尸进程，占用的资源在进程结束前都不会被回收，所以当一个线程运行完成后，我们应该等待回收资源。
// 我们可以注意到在上面的例子中，线程退出返回值和pthread_exit()的参数都是NULL，说明我们根本不关心线程的”死活“。
// 还有一个用处，在上面的例子中，我们都是在主线程中sleep()函数来防止新创建的线程还未结束，整个进程就结束，而现在我们可以用线程等待函数来达到这个目的。

// #include <pthread.h>
// int pthread_join(pthread_t thread, void ** retval);
// 描述：调用该函数的线程将挂起等待，直到id为thread的线程终止。thread线程以不同的方法终止，通过pthread_join() 得到的终止状态是不同的，主要有如下几种：
//        thread线程通过return 返回， retval 所指向的单元里存放的是 thread线程函数的返回值。
//        thread线程是被的线程调用pthread_cancel() 异常终止掉，retval 所指向的单元存放的是常数PTHREAD_CANCELED.
//        如果thread线程调用pthread_exit() 终止的，retval 所指向的单元存放的是传给pthread_exit的参数。

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* run_1(void *arg)
{
    printf(" I am thread 1 \n");
    return (void*)1; 
}

void* run_2(void *arg)
{
    printf(" I am thread 2 \n");

    pthread_exit((void*)2);
}


int main()
{
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, run_1, NULL);
    pthread_create(&tid2, NULL, run_2, NULL);

    void* retval_1;
    void* retval_2;
    pthread_join(tid1, &retval_1);
    pthread_join(tid2, &retval_2);

    printf(" thread 1 retval is  %u \n", (int*)retval_1);
    printf(" thread 2 retval is  %u \n", (int*)retval_2);

    return 0;
}
