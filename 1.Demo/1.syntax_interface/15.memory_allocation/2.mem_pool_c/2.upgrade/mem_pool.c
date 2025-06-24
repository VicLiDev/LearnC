/*************************************************************************
    > File Name: mem_pool.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 24 Jun 2025 06:07:46 PM CST
 ************************************************************************/

#define _GNU_SOURCE
#include "mem_pool.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <execinfo.h>
#include <time.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#define PROTECT_MAGIC 0xDEADC0DEDEADC0DE
#define TAIL_MAGIC    0xCAFEBABECAFEBABE
#define MAX_CALLSTACK 16
#define MAX_LABEL_LEN 32
#define MAX_SLAB_CLASSES 32
#define SLAB_MIN_SIZE 8
#define SLAB_MAX_SIZE 8192

static bool debug_mode = false;
static bool protect_mode = true;

typedef struct MemBlock {
    size_t size;
    atomic_bool freed;
    char label[MAX_LABEL_LEN];
    void*  real_ptr;
    struct MemBlock* next;
    void* callstack[MAX_CALLSTACK];
    int frames;
    time_t alloc_time;
    uint64_t guard_head;
} MemBlock;

typedef struct SlabClass {
    size_t block_size;
    MemBlock* free_list;
    pthread_mutex_t lock;
} SlabClass;

static size_t slab_sizes[MAX_SLAB_CLASSES];
static size_t slab_class_count = 0;
static SlabClass slab_classes[MAX_SLAB_CLASSES];
static MemBlock* active_list = NULL;
/* 使用 POSIX 线程库提供的默认参数，直接初始化锁 */
static pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;

static void init_slab_classes(void)
{
    size_t size = SLAB_MIN_SIZE;
    int i;

    for (i = 0; i < MAX_SLAB_CLASSES && size <= SLAB_MAX_SIZE; ++i) {
        slab_sizes[i] = size;
        slab_classes[i].block_size = size;
        pthread_mutex_init(&slab_classes[i].lock, NULL);
        size *= 2;
        slab_class_count++;
    }
}

static int find_slab_index(size_t size)
{
    for (int i = 0; i < slab_class_count; ++i) {
        if (size <= slab_sizes[i]) return i;
    }
    return -1;
}

void mem_pool_init(void)
{
    init_slab_classes();
}

void mem_pool_destroy(void)
{
    pthread_mutex_lock(&global_lock);
    MemBlock* cur = active_list;
    while (cur) {
        MemBlock* next = cur->next;
        free(cur->real_ptr);
        cur = next;
    }
    active_list = NULL;
    pthread_mutex_unlock(&global_lock);
}

void mem_pool_set_debug(int enabled)
{
    debug_mode = (enabled != 0);
}

void mem_pool_set_protection(int enabled)
{
    protect_mode = (enabled != 0);
}

void* mem_pool_alloc_label(size_t size, const char* label)
{
    int idx = find_slab_index(size);
    size_t actual_size = size;
    size_t total_size;
    void* raw = NULL;

    if (idx >= 0)
        actual_size = slab_classes[idx].block_size;
    total_size = sizeof(MemBlock) + actual_size + (protect_mode ? sizeof(uint64_t) : 0);

    if (idx >= 0) {
        pthread_mutex_lock(&slab_classes[idx].lock);
        if (slab_classes[idx].free_list) {
            MemBlock* blk = slab_classes[idx].free_list;
            slab_classes[idx].free_list = blk->next;
            raw = blk;
        }
        pthread_mutex_unlock(&slab_classes[idx].lock);
    }
    if (!raw)
        raw = malloc(total_size);
    if (!raw)
        return NULL;

    MemBlock* blk = (MemBlock*)raw;
    blk->size = size;
    blk->real_ptr = raw;
    atomic_init(&blk->freed, false);
    blk->alloc_time = time(NULL);
    blk->frames = 0;
    blk->guard_head = protect_mode ? PROTECT_MAGIC : 0;

    strncpy(blk->label, label ? label : "unknown", MAX_LABEL_LEN);
    blk->label[MAX_LABEL_LEN - 1] = '\0';

    if (debug_mode) {
        blk->frames = backtrace(blk->callstack, MAX_CALLSTACK);
    }
    if (protect_mode) {
        uint64_t* tail = (uint64_t*)((char*)raw + sizeof(MemBlock) + size);
        *tail = TAIL_MAGIC;
    }

    pthread_mutex_lock(&global_lock);
    blk->next = active_list;
    active_list = blk;
    pthread_mutex_unlock(&global_lock);

    return (void*)((char*)raw + sizeof(MemBlock));
}

void* mem_pool_alloc(size_t size)
{
    return mem_pool_alloc_label(size, NULL);
}

void mem_pool_free(void* ptr)
{
    if (!ptr) return;
    MemBlock* blk = (MemBlock*)((char*)ptr - sizeof(MemBlock));

    pthread_mutex_lock(&global_lock);

    if (atomic_load(&blk->freed)) {
        fprintf(stderr, "[mem_pool] Double free detected: %p\n", ptr);
        pthread_mutex_unlock(&global_lock);
        abort();
    }

    if (protect_mode && blk->guard_head != PROTECT_MAGIC) {
        fprintf(stderr, "[mem_pool] Buffer underrun: %p label=%s\n", ptr, blk->label);
        pthread_mutex_unlock(&global_lock);
        abort();
    }

    if (protect_mode) {
        uint64_t* tail = (uint64_t*)((char*)blk + sizeof(MemBlock) + blk->size);
        if (*tail != TAIL_MAGIC) {
            fprintf(stderr, "[mem_pool] Buffer overflow: %p label=%s\n", ptr, blk->label);
            pthread_mutex_unlock(&global_lock);
            abort();
        }
    }

    atomic_store(&blk->freed, true);

    // 从 active list 移除
    MemBlock **prev = &active_list;
    while (*prev) {
        if (*prev == blk) {
            /* 这里如果不使用*prev，而是使用prev，就无法达到修改链表的作用 */
            *prev = blk->next;
            break;
        }
        prev = &(*prev)->next;
    }
    pthread_mutex_unlock(&global_lock);

    int idx = find_slab_index(blk->size);
    if (idx >= 0) {
        pthread_mutex_lock(&slab_classes[idx].lock);
        blk->next = slab_classes[idx].free_list;
        slab_classes[idx].free_list = blk;
        pthread_mutex_unlock(&slab_classes[idx].lock);
    } else {
        free(blk->real_ptr);
    }
}

void mem_pool_dump_stats(void)
{
    int total = 0;
    size_t total_size = 0;

    pthread_mutex_lock(&global_lock);
    for (MemBlock* cur = active_list; cur; cur = cur->next) {
        if (!atomic_load(&cur->freed)) {
            total++;
            total_size += cur->size;
        }
    }
    printf("[mem_pool] Active blocks: %d | Total bytes: %zu\n", total, total_size);
    pthread_mutex_unlock(&global_lock);
}

void mem_pool_dump_leaks(void)
{
    int leaks = 0;

    pthread_mutex_lock(&global_lock);
    for (MemBlock* cur = active_list; cur; cur = cur->next) {
        if (!atomic_load(&cur->freed)) {
            leaks++;
            void* user_ptr = (void*)((char*)cur + sizeof(MemBlock));
            printf("[Leak] %p size=%zu label='%s' time=%s", user_ptr, cur->size, cur->label, ctime(&cur->alloc_time));
            if (debug_mode && cur->frames > 0) {
                char** symbols = backtrace_symbols(cur->callstack, cur->frames);
                for (int i = 0; i < cur->frames; ++i) {
                    printf("    %s\n", symbols[i]);
                }
                free(symbols);
            }
        }
    }
    if (leaks == 0)
        printf("[mem_pool] No memory leaks detected.\n");
    pthread_mutex_unlock(&global_lock);
}
