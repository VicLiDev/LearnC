/*************************************************************************
    > File Name: mem_pool.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 24 Jun 2025 06:07:46 PM CST
 ************************************************************************/

#include "mem_pool.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <execinfo.h>
#include <stdio.h>

#define SLAB_COUNT 8
static const size_t SLAB_SIZES[SLAB_COUNT] = { 32, 64, 128, 256, 512, 1024, 2048, 4096 };
#define GUARD_SIZE 16
#define GUARD_VAL  0xAB
#define MAX_BACKTRACE 10

typedef struct Block {
    void* real_ptr;
    void* user_ptr;
    size_t size;
    struct Block* next;

    void* backtrace[MAX_BACKTRACE];
    int bt_size;
} Block;

typedef struct Slab {
    size_t chunk_size;
    Block* free_list;
    size_t total;
    size_t in_use;
} Slab;

static Slab slabs[SLAB_COUNT];
static pthread_mutex_t pool_lock = PTHREAD_MUTEX_INITIALIZER;
static Block** active_blocks = NULL;
static size_t active_count = 0, active_capacity = 0;

static int slab_index(size_t size)
{
    for (int i = 0; i < SLAB_COUNT; ++i)
        if (SLAB_SIZES[i] >= size)
            return i;
    return -1;
}

static void push_active(Block* b)
{
    if (active_count >= active_capacity) {
        active_capacity = (active_capacity ? active_capacity * 2 : 64);
        active_blocks = realloc(active_blocks, active_capacity * sizeof(Block*));
    }
    active_blocks[active_count++] = b;
}

static void remove_active(Block* b)
{
    for (size_t i = 0; i < active_count; ++i) {
        if (active_blocks[i] == b) {
            active_blocks[i] = active_blocks[--active_count];
            return;
        }
    }
}

static void fill_guard(unsigned char* p)
{
    memset(p, GUARD_VAL, GUARD_SIZE);
}

static int check_guard(const unsigned char* p)
{
    for (int i = 0; i < GUARD_SIZE; ++i)
        if (p[i] != GUARD_VAL)
            return 0;
    return 1;
}

void mem_pool_init(void)
{
    for (int i = 0; i < SLAB_COUNT; ++i)
        slabs[i].chunk_size = SLAB_SIZES[i];
}

void* mem_pool_alloc(size_t size)
{
    pthread_mutex_lock(&pool_lock);

    int idx = slab_index(size);
    Block* blk = NULL;

    if (idx >= 0 && slabs[idx].free_list) {
        blk = slabs[idx].free_list;
        slabs[idx].free_list = blk->next;
        slabs[idx].in_use++;
    } else {
        blk = calloc(1, sizeof(Block));
        size_t total = size + GUARD_SIZE * 2;
        blk->real_ptr = malloc(total);
        blk->user_ptr = (unsigned char*)blk->real_ptr + GUARD_SIZE;
        blk->size = size;
        fill_guard(blk->real_ptr);
        fill_guard((unsigned char*)blk->user_ptr + size);

        if (idx >= 0)
            slabs[idx].total++;
    }

    blk->bt_size = backtrace(blk->backtrace, MAX_BACKTRACE);
    push_active(blk);

    pthread_mutex_unlock(&pool_lock);
    return blk->user_ptr;
}

void mem_pool_free(void* ptr)
{
    if (!ptr) return;
    pthread_mutex_lock(&pool_lock);

    Block* found = NULL;
    for (size_t i = 0; i < active_count; ++i) {
        if (active_blocks[i]->user_ptr == ptr) {
            found = active_blocks[i];
            remove_active(found);
            break;
        }
    }

    if (!found) {
        fprintf(stderr, "Invalid or double free detected\n");
        pthread_mutex_unlock(&pool_lock);
        return;
    }

    if (!check_guard((unsigned char*)found->real_ptr) ||
        !check_guard((unsigned char*)found->user_ptr + found->size)) {
        fprintf(stderr, "Buffer overflow detected on block size %zu\n", found->size);
    }

    int idx = slab_index(found->size);
    if (idx >= 0) {
        found->next = slabs[idx].free_list;
        slabs[idx].free_list = found;
        slabs[idx].in_use--;
    } else {
        free(found->real_ptr);
        free(found);
    }

    pthread_mutex_unlock(&pool_lock);
}

// Internal no-lock versions
static void __mem_pool_dump_stats_nolock(void)
{
    printf("\n[Slab Statistics]\n");
    for (int i = 0; i < SLAB_COUNT; ++i) {
        printf(" Slab %4zu bytes: total=%zu in-use=%zu free=%zu\n",
               slabs[i].chunk_size, slabs[i].total,
               slabs[i].in_use,
               slabs[i].total - slabs[i].in_use);
    }
}

static void __mem_pool_dump_leaks_nolock(void)
{
    if (active_count == 0) {
        printf("[Leak Check] No leaks detected.\n");
    } else {
        printf("[Leak Check] %zu unfreed blocks:\n", active_count);
        for (size_t i = 0; i < active_count; ++i) {
            Block* b = active_blocks[i];
            printf(" Leak: %p (%zu bytes)\n", b->user_ptr, b->size);
            char** symbols = backtrace_symbols(b->backtrace, b->bt_size);
            for (int j = 0; j < b->bt_size; ++j)
                printf("  %s\n", symbols[j]);
            free(symbols);
        }
    }
}

// Thread-safe wrappers
void mem_pool_dump_stats(void)
{
    pthread_mutex_lock(&pool_lock);
    __mem_pool_dump_stats_nolock();
    pthread_mutex_unlock(&pool_lock);
}

void mem_pool_dump_leaks(void)
{
    pthread_mutex_lock(&pool_lock);
    __mem_pool_dump_leaks_nolock();
    pthread_mutex_unlock(&pool_lock);
}

void mem_pool_destroy(void)
{
    pthread_mutex_lock(&pool_lock);

    __mem_pool_dump_stats_nolock();
    __mem_pool_dump_leaks_nolock();

    for (int i = 0; i < SLAB_COUNT; ++i) {
        Block* b = slabs[i].free_list;
        while (b) {
            Block* next = b->next;
            free(b->real_ptr);
            free(b);
            b = next;
        }
    }

    for (size_t i = 0; i < active_count; ++i) {
        free(active_blocks[i]->real_ptr);
        free(active_blocks[i]);
    }

    free(active_blocks);
    active_blocks = NULL;
    active_count = 0;
    active_capacity = 0;

    pthread_mutex_unlock(&pool_lock);
}

