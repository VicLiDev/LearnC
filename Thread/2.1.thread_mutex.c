// 互斥锁
//
// 初始化互斥
// 互斥使用pthread_mutex_t对象表示。正如Pthread API中的绝大多数对象，他表示提供给各种互斥接口的模糊结构。虽然你可以动态创建互斥，绝大多数使用方式是静态的：
// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//
//
// 加锁
// 加锁即获取锁，通过pthread_mutex_lock()实现：
// int pthread_mutex_lock(pthread_mutex_t *mutex);
// 成功调用 pthread_mutex_lock() 会阻塞调用的线程，直到由mutex指向的互斥锁变得可用。一旦互斥锁可用了，调用i线程就会被唤醒，函数返回0。如果在调用时互斥锁可用，
// 函数会立即返回。
// 出错时，函数可能返回的非0错误码。
// 尝试获取已经持有的互斥锁会导致死锁。另外调用方往往不会检查返回值。直接执行： pthread_mutex_lock(&mutex);
//
//
// 解锁
// #include <pthread.h>
// int pthread_mutex_unlock(pthread_mutex_t *mutex);
// 成功调用pthread_mutex_unlock()会释放由mutex所指向的互斥锁，并返回0.该调用不会阻塞，互斥锁可以立即释放。
// 出错时返回非零的错误码。
// 调用方往往不会检查返回值。直接执行： pthread_mutex_unlock(&mutex);


#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

static pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;

// 多个线程调用该函数时实现互斥功能
void printVal(int num)
{
	pthread_mutex_lock(&m_mutex);
	printf("============== I am thread %d ==============\n", num);
	for(int i = 0; i < 10; i++){
		printf("tid is: %u \n", pthread_self());
	}
	pthread_mutex_unlock(&m_mutex);
	return ;
}

void* run_1(void *arg)
{
    printVal(1);
    return (void*)1; 
}

void* run_2(void *arg)
{
    printVal(2);
    pthread_exit((void*)2);
}


int main()
{
	// initiate mutex
	pthread_mutex_init( &m_mutex, NULL ) ;
	
	pthread_t tid1, tid2;
	int ret;
	
	ret = pthread_create(&tid1, NULL, run_1, NULL);
    if(ret){
        errno = ret;
        perror("pthread_create");
        return -1;
    }
    ret = pthread_create(&tid2, NULL, run_2, NULL);
    if(ret){
        errno = ret;
        perror("pthread_create");
        return -1;
    }

    void* retval_1;
    void* retval_2;
    pthread_join(tid1, &retval_1);
    pthread_join(tid2, &retval_2);

    printf(" thread 1 retval is  %u \n", (int*)retval_1);
    printf(" thread 2 retval is  %u \n", (int*)retval_2);

	// destroy mutex
	pthread_mutex_destroy( &m_mutex ) ;

    return 0;
}
