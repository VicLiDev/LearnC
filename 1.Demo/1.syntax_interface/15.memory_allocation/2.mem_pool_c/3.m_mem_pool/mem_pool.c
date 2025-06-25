/*************************************************************************
    > File Name: mem_pool.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 24 Jun 2025 06:07:46 PM CST
 ************************************************************************/

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <execinfo.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "mem_pool.h"

#define PROTECT_MAGIC 0xDEADC0DEDEADC0DE
#define TAIL_MAGIC    0xCAFEBABECAFEBABE
#define MAX_CALLSTACK 16
#define MAX_LABEL_LEN 32
#define MAX_SLAB_CLASSES 32
#define SLAB_MIN_SIZE 8
#define SLAB_MAX_SIZE 8192

typedef enum ret_type_e {
    ret_ok = 0,
    ret_nok,
} ret_type;

typedef struct mem_block_t {
    size_t size;
    atomic_bool freed;
    char label[MAX_LABEL_LEN];
    void*  real_ptr;
    struct mem_block_t* next;
    void* callstack[MAX_CALLSTACK];
    int frames;
    time_t alloc_time;
    uint64_t guard_head;
} mem_blk;

typedef struct slab_class_t {
    size_t block_size;
    mem_blk *free_list;
    pthread_mutex_t lock;
} slab_class;

typedef struct mem_slab_pool_server_t {
    size_t slab_cls_cnt;
    slab_class slabs[MAX_SLAB_CLASSES];
    mem_blk *active_list;
    pthread_mutex_t glb_lock;
    bool debug_mode;
    bool protect_mode;
} mem_pool_srv;

static mem_pool_srv *mp_srv = NULL;

static int find_slab_index(size_t size)
{
    int i;

    for (i = 0; i < mp_srv->slab_cls_cnt; ++i) {
        if (size <= mp_srv->slabs[i].block_size)
            return i;
    }

    return -1;
}

void mem_pool_set_debug(int enabled)
{
    pthread_mutex_lock(&mp_srv->glb_lock);
    mp_srv->debug_mode = (enabled != 0);
    pthread_mutex_unlock(&mp_srv->glb_lock);
}

void mem_pool_set_protection(int enabled)
{
    pthread_mutex_lock(&mp_srv->glb_lock);
    mp_srv->protect_mode = (enabled != 0);
    pthread_mutex_unlock(&mp_srv->glb_lock);
}

void* mem_pool_alloc_label(size_t size, const char* label)
{
    int idx = find_slab_index(size);
    size_t actual_size = size;
    size_t total_size;
    void *raw = NULL;
    mem_blk *blk = NULL;
    mem_blk *tmp = NULL;

    if (idx >= 0)
        actual_size = mp_srv->slabs[idx].block_size;
    total_size = sizeof(mem_blk) + actual_size + (mp_srv->protect_mode ? sizeof(uint64_t) : 0);

    if (idx >= 0) {
        pthread_mutex_lock(&mp_srv->slabs[idx].lock);
        if (mp_srv->slabs[idx].free_list) {
            tmp = mp_srv->slabs[idx].free_list;
            mp_srv->slabs[idx].free_list = tmp->next;
            raw = tmp;
        }
        pthread_mutex_unlock(&mp_srv->slabs[idx].lock);
    }
    if (!raw)
        raw = malloc(total_size);
    if (!raw)
        return NULL;

    blk = (mem_blk*)raw;
    blk->size = actual_size;
    blk->real_ptr = raw;
    atomic_init(&blk->freed, false);
    blk->alloc_time = time(NULL);
    blk->frames = 0;
    blk->guard_head = mp_srv->protect_mode ? PROTECT_MAGIC : 0;

    strncpy(blk->label, label ? label : "unknown", MAX_LABEL_LEN);
    blk->label[MAX_LABEL_LEN - 1] = '\0';

    if (mp_srv->debug_mode) {
        blk->frames = backtrace(blk->callstack, MAX_CALLSTACK);
    }
    if (mp_srv->protect_mode) {
        uint64_t* tail = (uint64_t*)((char*)raw + sizeof(mem_blk) + actual_size - sizeof(uint64_t));
        *tail = TAIL_MAGIC;
    }

    pthread_mutex_lock(&mp_srv->glb_lock);
    blk->next = mp_srv->active_list;
    mp_srv->active_list = blk;
    pthread_mutex_unlock(&mp_srv->glb_lock);

    return (void*)((char*)raw + sizeof(mem_blk));
}

void* mem_pool_alloc(size_t size)
{
    return mem_pool_alloc_label(size, NULL);
}

void mem_pool_free(void* ptr)
{
    if (!ptr)
        return;

    mem_blk *blk = (mem_blk*)((char*)ptr - sizeof(mem_blk));
    mem_blk **prev = NULL;
    int slab_idx;

    pthread_mutex_lock(&mp_srv->glb_lock);

    if (atomic_load(&blk->freed)) {
        fprintf(stderr, "[mem_pool] Double free detected: %p\n", ptr);
        pthread_mutex_unlock(&mp_srv->glb_lock);
        abort();
    }

    if (mp_srv->protect_mode && blk->guard_head != PROTECT_MAGIC) {
        fprintf(stderr, "[mem_pool] Buffer underrun: %p label=%s\n", ptr, blk->label);
        pthread_mutex_unlock(&mp_srv->glb_lock);
        abort();
    }

    if (mp_srv->protect_mode) {
        uint64_t* tail = (uint64_t*)((char*)blk + sizeof(mem_blk) + blk->size - sizeof(uint64_t));

        if (*tail != TAIL_MAGIC) {
            fprintf(stderr, "[mem_pool] Buffer overflow: %p label=%s\n", ptr, blk->label);
            pthread_mutex_unlock(&mp_srv->glb_lock);
            abort();
        }
    }

    atomic_store(&blk->freed, true);

    // 从 active list 移除
    prev = &mp_srv->active_list;
    while (*prev) {
        if (*prev == blk) {
            /* 这里如果不使用*prev，而是使用prev，就无法达到修改链表的作用 */
            *prev = blk->next;
            break;
        }
        prev = &(*prev)->next;
    }
    pthread_mutex_unlock(&mp_srv->glb_lock);

    slab_idx = find_slab_index(blk->size);
    if (slab_idx >= 0) {
        pthread_mutex_lock(&mp_srv->slabs[slab_idx].lock);
        blk->next = mp_srv->slabs[slab_idx].free_list;
        mp_srv->slabs[slab_idx].free_list = blk;
        pthread_mutex_unlock(&mp_srv->slabs[slab_idx].lock);
    } else {
        free(blk->real_ptr);
    }
}

static void __mem_pool_dump_stats_nolock()
{
    int total = 0;
    size_t total_size = 0;
    mem_blk *cur = NULL;

    for (cur = mp_srv->active_list; cur; cur = cur->next) {
        if (!atomic_load(&cur->freed)) {
            total++;
            total_size += cur->size;
        }
    }
    printf("[mem_pool] Active blocks: %d | Total bytes: %zu\n", total, total_size);
}

static void __mem_pool_dump_leaks_nolock()
{
    int leaks = 0;
    mem_blk *cur = NULL;
    int i;

    for (cur = mp_srv->active_list; cur; cur = cur->next) {
        if (!atomic_load(&cur->freed)) {
            void* user_ptr = (void*)((char*)cur + sizeof(mem_blk));

            leaks++;
            printf("[Leak] %p size=%zu label='%s' time=%s", user_ptr, cur->size, cur->label, ctime(&cur->alloc_time));
            if (mp_srv->debug_mode && cur->frames > 0) {
                char** symbols = backtrace_symbols(cur->callstack, cur->frames);

                for (i = 0; i < cur->frames; ++i)
                    printf("    %s\n", symbols[i]);
                free(symbols);
            }
        }
    }
    if (leaks == 0)
        printf("[mem_pool] No memory leaks detected.\n");
}

void mem_pool_dump_stats()
{
    pthread_mutex_lock(&mp_srv->glb_lock);
    __mem_pool_dump_stats_nolock();
    pthread_mutex_unlock(&mp_srv->glb_lock);
}

void mem_pool_dump_leaks()
{
    pthread_mutex_lock(&mp_srv->glb_lock);
    __mem_pool_dump_leaks_nolock();
    pthread_mutex_unlock(&mp_srv->glb_lock);
}

__attribute__((constructor(101))) void mem_pool_init()
{
    size_t size = SLAB_MIN_SIZE;
    int i;
    mem_pool_srv *srv_tmp = (mem_pool_srv *)calloc(1, sizeof(mem_pool_srv));

    srv_tmp->slab_cls_cnt = 0;
    srv_tmp->active_list = NULL;
    pthread_mutex_init(&srv_tmp->glb_lock, NULL);
    srv_tmp->debug_mode = false;
    srv_tmp->protect_mode = true;

    for (i = 0; i < MAX_SLAB_CLASSES && size <= SLAB_MAX_SIZE; i++) {
        srv_tmp->slabs[i].block_size = size;
        pthread_mutex_init(&srv_tmp->slabs[i].lock, NULL);
        size *= 2;
        srv_tmp->slab_cls_cnt++;
    }

    mp_srv = srv_tmp;
}

__attribute__((destructor(101))) void mem_pool_destroy()
{
    mem_blk *cur = NULL;;
    mem_blk *next = NULL;
    int i;

    pthread_mutex_lock(&mp_srv->glb_lock);
    for (i = 0; i < mp_srv->slab_cls_cnt; i++) {
        cur = mp_srv->slabs[i].free_list;
        while (cur) {
            next = cur->next;
            free(cur->real_ptr);
            cur = next;
        }
    }

    cur = mp_srv->active_list;
    if (cur)
        __mem_pool_dump_leaks_nolock();
    while (cur) {
        next = cur->next;
        free(cur->real_ptr);
        cur = next;
    }
    mp_srv->active_list = NULL;
    pthread_mutex_unlock(&mp_srv->glb_lock);

    free(mp_srv);
}
