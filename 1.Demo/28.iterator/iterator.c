/*************************************************************************
    > File Name: iterator.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Sat 24 May 2025 08:55:40 AM CST
 ************************************************************************/

#include <stdio.h>

// 定义一个数组迭代器
typedef struct {
    int *array;
    size_t current;
    size_t size;
} ArrayIterator;

// 初始化迭代器
void init_iterator(ArrayIterator *it, int *array, size_t size)
{
    it->array = array;
    it->current = 0;
    it->size = size;
}

// 检查是否有下一个元素
int has_next(ArrayIterator *it)
{
    return it->current < it->size;
}

// 获取下一个元素
int next(ArrayIterator *it)
{
    return it->array[it->current++];
}

int main()
{
    int arr[] = {1, 2, 3, 4, 5};
    ArrayIterator it;
    init_iterator(&it, arr, sizeof(arr)/sizeof(arr[0]));

    while (has_next(&it)) {
        printf("%d ", next(&it));
    }
    // 输出: 1 2 3 4 5

    return 0;
}
