/*************************************************************************
    > File Name: explicit_free_list.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 24 Jun 2025 05:22:32 PM CST
 ************************************************************************/

/*
 * 显式空闲列表
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POOL_SIZE 1024 * 10  // 10KB
#define ALIGN4(x) (((x) + 3) & ~3)

typedef struct Block {
    size_t size;
    int free;
    struct Block* next;
    struct Block* prev;
} Block;

char* memory_pool;
Block* free_list = NULL;

void init_memory()
{
    memory_pool = (char*)malloc(POOL_SIZE);
    free_list = (Block*)memory_pool;
    free_list->size = POOL_SIZE - sizeof(Block);
    free_list->free = 1;
    free_list->next = NULL;
    free_list->prev = NULL;
}

void remove_from_free_list(Block* block)
{
    if (block->prev) block->prev->next = block->next;
    if (block->next) block->next->prev = block->prev;
    if (block == free_list) free_list = block->next;
}

void add_to_free_list(Block* block)
{
    block->next = free_list;
    if (free_list) free_list->prev = block;
    block->prev = NULL;
    free_list = block;
}

void* my_malloc(size_t size)
{
    size = ALIGN4(size);
    Block* curr = free_list;

    while (curr) {
        if (curr->free && curr->size >= size) {
            if (curr->size > size + sizeof(Block) + 4) {
                Block* new_block = (Block*)((char*)curr + sizeof(Block) + size);
                new_block->size = curr->size - size - sizeof(Block);
                new_block->free = 1;
                new_block->next = NULL;
                new_block->prev = NULL;
                add_to_free_list(new_block);

                curr->size = size;
            }

            curr->free = 0;
            remove_from_free_list(curr);
            return (void*)((char*)curr + sizeof(Block));
        }
        curr = curr->next;
    }
    return NULL;
}

void my_free(void* ptr)
{
    if (!ptr) return;

    Block* block = (Block*)((char*)ptr - sizeof(Block));
    block->free = 1;
    add_to_free_list(block);

    // 合并相邻空闲块（相对地址判断）
    char* end = memory_pool + POOL_SIZE;
    Block* curr = (Block*)memory_pool;
    while ((char*)curr < end) {
        char* next_ptr = (char*)curr + sizeof(Block) + curr->size;
        if (next_ptr >= end) break;
        Block* next = (Block*)next_ptr;

        if (curr->free && next->free) {
            remove_from_free_list(next);
            curr->size += sizeof(Block) + next->size;
        } else {
            curr = next;
        }
    }
}

void print_state()
{
    printf("\n[显式空闲列表状态]\n");
    Block* curr = (Block*)memory_pool;
    char* end = memory_pool + POOL_SIZE;
    int i = 0;

    while ((char*)curr < end) {
        printf("块 %d: 大小=%zu 状态=%s\n", i++, curr->size, curr->free ? "空闲" : "分配");
        curr = (Block*)((char*)curr + sizeof(Block) + curr->size);
    }
}

int main()
{
    init_memory();

    void* a = my_malloc(128);
    void* b = my_malloc(256);
    void* c = my_malloc(64);

    print_state();

    my_free(b);
    print_state();

    my_free(a);
    my_free(c);
    print_state();

    free(memory_pool);
    return 0;
}

