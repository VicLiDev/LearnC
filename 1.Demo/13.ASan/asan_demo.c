/*************************************************************************
    > File Name: asan_demo.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 14 May 2024 03:13:53 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>

int main()
{
    // 故意越界访问
    {
        int nums[] = {1, 2, 3};
        printf("Out-of-bounds access index 3: %d\n", nums[3]);
    }

    // 访问野指针
    {
        int *ptr = (int *)malloc(3 * sizeof(int));
        free(ptr);
        printf("Accessing wild pointers: %d\n", *ptr);
    }

    // 内存泄漏
    {
        int *ptr = (int *)malloc(3 * sizeof(int));
    }

    return 0;
}
