// 线程分离
// 无论何时，一个线程是可结合(joinable )的或者是分离(detached)的。
// 当线程属于可结合时，它能够被其他线程join或者cancel回收资源。相反一个已经处于分离的线程是不能被join或cancel，资源会在终止时自动释放。
// 创建一个线程，默认是可结合的，为了防止资源的泄露，我们可以显示的调用pthread_join() 回收资源。对一个处于可结合的线程调用pthread_join()后，可以将线程置于分离状态。不能对同一个线程调用两个join，对一个已经分离的线程调用join会返回错误号。
// 其实在上面的例子中，已经有过通过join将一个线程分离，但是当在一个线程中通过调用pthread_join()来回收资源时，调用者就会被阻塞，如果需要回收的线程数目过多时，效率就大大下降。比如在一个Web 服务器中， 主线程为每一个请求创建一个线程去响应动作，我们并不希望主线程也为了回收资源而被阻塞，因为可能在阻塞的同时有新的请求，我们可以再使用下面的方法，让线程办完事情后自动回收资源。
// 1 ). 在子线程中调用pthread_detach( pthread_self() )。 
// 2 ).在主线程中调用pthread_detach( tid )。

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>     // sleep()

void* run(void * arg)
{
    pthread_detach( pthread_self());
    printf("I will detach .. \n");
    return NULL;
}

int main()
{
    pthread_t tid1;
    pthread_create(&tid1, NULL, run, NULL);

    sleep(1); // 因为主线程不会挂起等待，为了保证子线程先执行完分离，让主线程先等待1s
    int ret = 0;
    ret =  pthread_join(tid1, NULL);
    if( ret == 0)
    {
        printf(" join sucess. \n");
    }
    else
    {
        printf(" join failed. \n");
    }
    return 0;
}
