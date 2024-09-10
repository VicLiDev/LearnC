/*
 * 一个不错的博客 https://www.cnblogs.com/CodingUniversal/p/7396671.html
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int glob=10;

/* 进程退出时调用函数 */
static void my_exit1(void)
{
    printf("pid=%d first exit handler\n",getpid());
}

static void my_exit2(void)
{
    printf("pid=%d second exit handler\n",getpid());
}

int main()
{
    int local;
    pid_t pid;
    local=8;

    /*
     * atexit函数是一个特殊的函数，它是在正常程序退出时调用的函数，我们把他叫为
     * 登记函数（函数原型：int atexit (void (*)(void))）为进程注册的退出时调用
     * 函数也会被子进程共享，先注册的后调用
     */
    if(atexit(my_exit1)!=0) {
        perror("atexit");
    }

    if(atexit(my_exit2)!=0){
        perror("atexit");
    }

    if((pid=fork())==0){
        /*
         * 子进程执行某个任务完后尽量使用exit退出，不然，若父进程中创建的子进程
         * 位于循环中，可能会引起未知的行为
         */
        printf("child pid is %d\n",getpid());
    }
    else if(pid>0){
        sleep(10);
        glob++;
        local--;
        printf("father pid is %d\n",getpid());       
    }
    else{
        perror("fork");
    }
    /* 这段代码父子进程共享 */
    printf("pid=%d,glob=%d,localar=%d\n",getpid(),glob,local);

    /* 也可以使用exit(0) */
    return 0;
}
