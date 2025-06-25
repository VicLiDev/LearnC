/*************************************************************************
    > File Name: dump_stack.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Thu Sep  8 17:23:39 2022
 ************************************************************************/

/*
 * 1. backtrace
 * int backtrace(void **buffer, int size);
 * 功能：获取当前调用栈的函数地址。
 * 参数：
 *   buffer：指针数组，用于存放调用栈中各层函数的返回地址。
 *   size：buffer 中可存储的最大地址个数。
 * 返回值：实际写入的地址个数。
 * 
 * 2. backtrace_symbols
 * char **backtrace_symbols(void *const *buffer, int size);
 * 功能：将 backtrace 获取的函数地址转化为可读的函数名字符串。
 * 参数：
 *   buffer：backtrace 生成的地址数组。
 *   size：数组中地址的个数。
 * 返回值：字符串数组，每个元素是一个函数调用的描述（包括函数名、偏移等）。
 * 注意：该函数返回的是动态分配的内存（通常是 malloc），应使用 free 释放。
 */

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>


static void printStack(void)
{
#define STACK_SIZE 32
    void *trace[STACK_SIZE];
    size_t size = backtrace(trace, STACK_SIZE);
    char **symbols = backtrace_symbols(trace, size);

    for (size_t i = 0; i < size; i++) {
        printf("%zu ---> %s\n", i, symbols[i]);
    }

    free(symbols);
}

void test(int idx)
{
    if (idx > 10)
        return;

    printf("======> stack idx: %d\n", idx);
    printStack();
    printf("\n");

    test(idx + 1);
}

int main()
{
    test(0);
    return 0;
}

