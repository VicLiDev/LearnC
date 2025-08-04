/*************************************************************************
    > File Name: 16.array.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Mon 04 Aug 2025 05:31:21 PM CST
 ************************************************************************/

#include <stdio.h>

#include <stdio.h>

#define SIZE 10

typedef struct {
    int id;
    const char *name;
} Person;

// 静态初始化结构体数组，只对指定索引赋值
Person people[SIZE] = {
    [2] = { .id = 101, .name = "Alice" },
    [5] = { .id = 205, .name = "Bob" },
    [8] = { .id = 308, .name = "Charlie" }
};

int main()
{
    for (int i = 0; i < SIZE; ++i) {
        if (people[i].id != 0) {
            printf("Index %d: ID = %d, Name = %s\n", i, people[i].id, people[i].name);
        }
    }
    return 0;
}

