/*************************************************************************
    > File Name: 14.long_jmp.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Sat Jul 27 19:11:38 2024
 ************************************************************************/

/*
 * longjmp的作用就是让程序的执行流回到当初setjmp返回的时刻，并且返回由longjmp指定
 * 的返回值（参数2）
 */

#include <stdio.h>
#include <setjmp.h>

jmp_buf b;

void m_func()
{
    longjmp(b, 1);
}

int main()
{
    if (setjmp(b))
        printf("World!");
    else {
        printf("Hello ");
        m_func();
    }
}
