/*************************************************************************
    > File Name: 4.slab_style_fixed_size_bins.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 24 Jun 2025 05:26:49 PM CST
 ************************************************************************/

/*
 * 桶式分配器（Fixed-size Bins / Slab-style）
 *
 * 核心思想：
 * 只支持 固定大小（或若干离散尺寸） 的块，每种大小一个空闲链表。非常高效，
 * 适合嵌入式和对象池。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POOL_SIZE 1024 * 4
#define CHUNK_32_SIZE 32
#define CHUNK_64_SIZE 64
#define NUM_32 50
#define NUM_64 30

typedef struct Chunk {
    struct Chunk* next;
} Chunk;

char* memory_pool;
Chunk* free_list_32 = NULL;
Chunk* free_list_64 = NULL;

void init_memory()
{
    memory_pool = (char*)malloc(POOL_SIZE);

    // 初始化32B桶
    char* ptr = memory_pool;
    for (int i = 0; i < NUM_32; i++) {
        Chunk* chunk = (Chunk*)ptr;
        chunk->next = free_list_32;
        free_list_32 = chunk;
        ptr += CHUNK_32_SIZE;
    }

    // 初始化64B桶
    for (int i = 0; i < NUM_64; i++) {
        Chunk* chunk = (Chunk*)ptr;
        chunk->next = free_list_64;
        free_list_64 = chunk;
        ptr += CHUNK_64_SIZE;
    }
}

void* alloc_32()
{
    if (!free_list_32) return NULL;
    Chunk* chunk = free_list_32;
    free_list_32 = chunk->next;
    return chunk;
}

void* alloc_64()
{
    if (!free_list_64) return NULL;
    Chunk* chunk = free_list_64;
    free_list_64 = chunk->next;
    return chunk;
}

void free_32(void* ptr)
{
    Chunk* chunk = (Chunk*)ptr;
    chunk->next = free_list_32;
    free_list_32 = chunk;
}

void free_64(void* ptr)
{
    Chunk* chunk = (Chunk*)ptr;
    chunk->next = free_list_64;
    free_list_64 = chunk;
}

void print_state()
{
    int count_32 = 0, count_64 = 0;
    Chunk* c;

    for (c = free_list_32; c; c = c->next) count_32++;
    for (c = free_list_64; c; c = c->next) count_64++;

    printf("\n[桶式分配器状态]  32B桶：%d个空闲，64B桶：%d个空闲\n", count_32, count_64);
}

int main()
{
    init_memory();

    void* a = alloc_32();
    void* b = alloc_64();
    void* c = alloc_64();

    print_state();

    free_64(b);
    print_state();

    free_32(a);
    free_64(c);
    print_state();

    free(memory_pool);
    return 0;
}

