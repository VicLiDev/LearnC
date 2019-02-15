//使用 g++ 编译需要增加 -lpthread 库。  GNU/Linux 实现了 POSIX 标准线程 API(所谓 pthreads)。所有线程函数和数据类型
//                                  都在 <pthread.h> 头文件中声明。这些线程相关的函数没有被包含在 C 标准库中,而是在
//                                  libpthread 中,所以当链接程序的时候需在命令行中加入 -lpthread 以确保能正确链接。
//
//一篇很不错的博客: https://blog.csdn.net/a1414345/article/details/70947396
//线程主要有三种状态分别是就绪、阻塞、运行。
//就绪：线程具备运行的所有条件，逻辑上已可以运行，在等待处理机。
//阻塞：指线程在等待某一事件的发生，如I/O操作。
//运行：占有处理器正在运行。
//
//函数 pthread_create 负责创建新线程。你需要给它提供如下信息:
//1、一个指向 pthread_t 类型变量的指针;新线程的线程 ID 将存储在这里。
//2、一个指向 线程属性( thread attribute ) 对象的指针。这个对象控制着新线程与程序其它部分交互的具体细节。如果传递 NULL 作为
//线程属性,新线程将被赋予一组默认线程属性。
//3、一个指向线程函数的指针。这是一个普通的函数指针,类型如下:
//void* (*) (void*)
//4、一个线程参数,类型 void*。不论你传递什么值作为这个参数,当线程开始执行的时候,它都会被直接传递给新的线程。

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
