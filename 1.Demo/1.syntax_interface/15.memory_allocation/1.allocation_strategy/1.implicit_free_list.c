/*************************************************************************
    > File Name: implicit_free_list.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 24 Jun 2025 05:21:11 PM CST
 ************************************************************************/

/*
 * 隐式空闲列表
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POOL_SIZE 1024 * 10  // 10KB
#define ALIGN4(x) (((x) + 3) & ~3)

typedef struct {
    size_t size;   // 块大小，不包含头部
    int free;      // 1=空闲, 0=已分配
} BlockHeader;

char* memory_pool;
size_t used_size = 0;

void init_memory()
{
    memory_pool = (char*)malloc(POOL_SIZE);
    BlockHeader* init_block = (BlockHeader*)memory_pool;
    init_block->size = POOL_SIZE - sizeof(BlockHeader);
    init_block->free = 1;
}

// 遍历每个块
void* my_malloc(size_t size)
{
    size = ALIGN4(size);
    char* ptr = memory_pool;

    while (ptr < memory_pool + POOL_SIZE) {
        BlockHeader* block = (BlockHeader*)ptr;
        if (block->free && block->size >= size) {
            size_t remaining = block->size - size;
            if (remaining > sizeof(BlockHeader) + 4) {
                // 分裂块
                BlockHeader* next = (BlockHeader*)(ptr + sizeof(BlockHeader) + size);
                next->size = remaining - sizeof(BlockHeader);
                next->free = 1;
                block->size = size;
            }
            block->free = 0;
            return ptr + sizeof(BlockHeader);
        }
        ptr += sizeof(BlockHeader) + block->size;
    }
    return NULL;
}

void my_free(void* ptr)
{
    if (!ptr) return;
    BlockHeader* block = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
    block->free = 1;

    // 合并相邻空闲块
    char* curr = memory_pool;
    while (curr < memory_pool + POOL_SIZE) {
        BlockHeader* b = (BlockHeader*)curr;
        char* next_ptr = curr + sizeof(BlockHeader) + b->size;
        if (next_ptr >= memory_pool + POOL_SIZE) break;
        BlockHeader* next = (BlockHeader*)next_ptr;

        if (b->free && next->free) {
            b->size += sizeof(BlockHeader) + next->size;
        } else {
            curr = next_ptr;
        }
    }
}

void print_state()
{
    char* ptr = memory_pool;
    int i = 0;
    printf("\n[隐式空闲列表状态]\n");
    while (ptr < memory_pool + POOL_SIZE) {
        BlockHeader* block = (BlockHeader*)ptr;
        printf("块 %d: 大小=%zu 状态=%s\n", i++, block->size, block->free ? "空闲" : "分配");
        ptr += sizeof(BlockHeader) + block->size;
    }
}

int main()
{
    init_memory();

    void* a = my_malloc(100);
    void* b = my_malloc(200);
    void* c = my_malloc(300);

    print_state();

    my_free(b);
    print_state();

    my_free(a);
    my_free(c);
    print_state();

    free(memory_pool);
    return 0;
}

