/*************************************************************************
    > File Name: demo.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Fri 10 Jan 15:24:55 2025
 ************************************************************************/

#include <stdio.h>

// 定义宏辅助工具
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// 编译时信息
#pragma message("Compiling the demo file...")
#pragma message("This message is shown during compilation.")

// 使用 _Pragma 动态构造消息
#define COMPILER_MESSAGE(msg) _Pragma(TOSTRING(message(msg)))
COMPILER_MESSAGE("Using dynamic _Pragma to show this message.")

// 打印编译器内置宏信息
#pragma message("Compiler Info:")
#pragma message("File: " __FILE__ ", Line: " TOSTRING(__LINE__))
#pragma message("Compiled on: " __DATE__ " at " __TIME__)

// 使用警告提示
#ifdef DEBUG
#warning "Debug mode is enabled. Remember to disable it for production."
#endif

// 使用错误检查配置是否定义
#ifndef CONFIG_VALUE
#error "CONFIG_VALUE is not defined! Please define it to proceed."
#endif

int main()
{
    printf("Hello, World!\n");

    // 运行时输出
    printf("File: %s, Line: %d\n", __FILE__, __LINE__);
    printf("Compiled on: %s at %s\n", __DATE__, __TIME__);

#ifdef DEBUG
    printf("Debug mode is enabled.\n");
#endif

    return 0;
}

