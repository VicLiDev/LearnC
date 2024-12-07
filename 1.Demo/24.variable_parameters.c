/*************************************************************************
    > File Name: 24.variable_parameters.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Sat  7 Dec 16:19:02 2024
 ************************************************************************/

#include <stdio.h>
#include <stdarg.h>

// 1. 变参宏的使用
// 通过 ##__VA_ARGS__ 去除当宏无额外参数时的逗号问题，`##__VA_ARGS__` 在展开的
// 时候，因为 ‘##’ 找不到连接对象，会将‘##’之前的空白字符和‘,’（逗号）删除，
#define LOG(level, fmt, ...) printf("[" #level "] " fmt "\n", ##__VA_ARGS__)

// 2. 使用`#`将参数转为字符串
#define PRINT_VARIABLE_NAME_AND_VALUE(var) printf(#var " = %d\n", var)

// 3. 使用`##`连接符
#define CREATE_FUNCTION(name) \
    void func_##name() \
    { \
        printf("Function " #name " called.\n"); \
    }

// 4. 变参函数的使用
void print_sum(const char *message, int count, ...)
{
    int sum = 0;
    va_list args;

    // 准备访问可变参数
    va_start(args, count);
    for (int i = 0; i < count; ++i) {
        sum += va_arg(args, int);
    }
    // 完成处理可变参数
    va_end(args);

    printf("%s: Sum = %d\n", message, sum);
}

// 使用 `##` 定义的函数
CREATE_FUNCTION(test)

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // 使用变参宏
    LOG(INFO, "This is an informational message.");
    LOG(WARNING, "This is a warning with code %d", 404);
    printf("\n");

    // 使用 `#` 将参数转为字符串
    int x = 42;
    PRINT_VARIABLE_NAME_AND_VALUE(x);
    printf("\n");

    // 使用 `##` 拼接符
    func_test();
    printf("\n");

    // 使用变参函数
    print_sum("Adding numbers", 4, 1, 2, 3, 4);
    printf("\n");

    return 0;
}

