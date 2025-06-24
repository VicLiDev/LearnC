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
#include <execinfo.h>


static int find_slab_index(mem_pool_ctx *mp_ctx, size_t size)
{
    int i;

    for (i = 0; i < mp_ctx->slab_cls_cnt; ++i) {
        if (size <= mp_ctx->slabs[i].block_size)
            return i;
    }

    return -1;
}

void mem_pool_init(mem_pool_ctx **mp_ctx)
{
    size_t size = SLAB_MIN_SIZE;
    int i;
    mem_pool_ctx *ctx_tmp = (mem_pool_ctx *)calloc(1, sizeof(mem_pool_ctx));

    ctx_tmp->slab_cls_cnt = 0;
    ctx_tmp->active_list = NULL;
    pthread_mutex_init(&ctx_tmp->glb_lock, NULL);
    ctx_tmp->debug_mode = false;
    ctx_tmp->protect_mode = true;

    for (i = 0; i < MAX_SLAB_CLASSES && size <= SLAB_MAX_SIZE; i++) {
        ctx_tmp->slabs[i].block_size = size;
        pthread_mutex_init(&ctx_tmp->slabs[i].lock, NULL);
        size *= 2;
        ctx_tmp->slab_cls_cnt++;
    }

    *mp_ctx = ctx_tmp;
}

void mem_pool_destroy(mem_pool_ctx *mp_ctx)
{
    mem_blk *cur = NULL;;
    mem_blk *next = NULL;

    pthread_mutex_lock(&mp_ctx->glb_lock);
    cur = mp_ctx->active_list;
    while (cur) {
        next = cur->next;
        free(cur->real_ptr);
        cur = next;
    }
    mp_ctx->active_list = NULL;
    pthread_mutex_unlock(&mp_ctx->glb_lock);

    free(mp_ctx);
}

void mem_pool_set_debug(mem_pool_ctx *mp_ctx, int enabled)
{
    pthread_mutex_lock(&mp_ctx->glb_lock);
    mp_ctx->debug_mode = (enabled != 0);
    pthread_mutex_unlock(&mp_ctx->glb_lock);
}

void mem_pool_set_protection(mem_pool_ctx *mp_ctx, int enabled)
{
    pthread_mutex_lock(&mp_ctx->glb_lock);
    mp_ctx->protect_mode = (enabled != 0);
    pthread_mutex_unlock(&mp_ctx->glb_lock);
}

void* mem_pool_alloc_label(mem_pool_ctx *mp_ctx, size_t size, const char* label)
{
    int idx = find_slab_index(mp_ctx, size);
    size_t actual_size = size;
    size_t total_size;
    void *raw = NULL;
    mem_blk *blk = NULL;
    mem_blk *tmp = NULL;

    if (idx >= 0)
        actual_size = mp_ctx->slabs[idx].block_size;
    total_size = sizeof(mem_blk) + actual_size + (mp_ctx->protect_mode ? sizeof(uint64_t) : 0);

    if (idx >= 0) {
        pthread_mutex_lock(&mp_ctx->slabs[idx].lock);
        if (mp_ctx->slabs[idx].free_list) {
            tmp = mp_ctx->slabs[idx].free_list;
            mp_ctx->slabs[idx].free_list = tmp->next;
            raw = tmp;
        }
        pthread_mutex_unlock(&mp_ctx->slabs[idx].lock);
    }
    if (!raw)
        raw = malloc(total_size);
    if (!raw)
        return NULL;

    blk = (mem_blk*)raw;
    blk->size = size;
    blk->real_ptr = raw;
    atomic_init(&blk->freed, false);
    blk->alloc_time = time(NULL);
    blk->frames = 0;
    blk->guard_head = mp_ctx->protect_mode ? PROTECT_MAGIC : 0;

    strncpy(blk->label, label ? label : "unknown", MAX_LABEL_LEN);
    blk->label[MAX_LABEL_LEN - 1] = '\0';

    if (mp_ctx->debug_mode) {
        blk->frames = backtrace(blk->callstack, MAX_CALLSTACK);
    }
    if (mp_ctx->protect_mode) {
        uint64_t* tail = (uint64_t*)((char*)raw + sizeof(mem_blk) + size);
        *tail = TAIL_MAGIC;
    }

    pthread_mutex_lock(&mp_ctx->glb_lock);
    blk->next = mp_ctx->active_list;
    mp_ctx->active_list = blk;
    pthread_mutex_unlock(&mp_ctx->glb_lock);

    return (void*)((char*)raw + sizeof(mem_blk));
}

void* mem_pool_alloc(mem_pool_ctx *mp_ctx, size_t size)
{
    return mem_pool_alloc_label(mp_ctx, size, NULL);
}

void mem_pool_free(mem_pool_ctx *mp_ctx, void* ptr)
{
    if (!ptr)
        return;

    mem_blk *blk = (mem_blk*)((char*)ptr - sizeof(mem_blk));
    mem_blk **prev = NULL;
    int slab_idx;

    pthread_mutex_lock(&mp_ctx->glb_lock);

    if (atomic_load(&blk->freed)) {
        fprintf(stderr, "[mem_pool] Double free detected: %p\n", ptr);
        pthread_mutex_unlock(&mp_ctx->glb_lock);
        abort();
    }

    if (mp_ctx->protect_mode && blk->guard_head != PROTECT_MAGIC) {
        fprintf(stderr, "[mem_pool] Buffer underrun: %p label=%s\n", ptr, blk->label);
        pthread_mutex_unlock(&mp_ctx->glb_lock);
        abort();
    }

    if (mp_ctx->protect_mode) {
        uint64_t* tail = (uint64_t*)((char*)blk + sizeof(mem_blk) + blk->size);

        if (*tail != TAIL_MAGIC) {
            fprintf(stderr, "[mem_pool] Buffer overflow: %p label=%s\n", ptr, blk->label);
            pthread_mutex_unlock(&mp_ctx->glb_lock);
            abort();
        }
    }

    atomic_store(&blk->freed, true);

    // 从 active list 移除
    prev = &mp_ctx->active_list;
    while (*prev) {
        if (*prev == blk) {
            /* 这里如果不使用*prev，而是使用prev，就无法达到修改链表的作用 */
            *prev = blk->next;
            break;
        }
        prev = &(*prev)->next;
    }
    pthread_mutex_unlock(&mp_ctx->glb_lock);

    slab_idx = find_slab_index(mp_ctx, blk->size);
    if (slab_idx >= 0) {
        pthread_mutex_lock(&mp_ctx->slabs[slab_idx].lock);
        blk->next = mp_ctx->slabs[slab_idx].free_list;
        mp_ctx->slabs[slab_idx].free_list = blk;
        pthread_mutex_unlock(&mp_ctx->slabs[slab_idx].lock);
    } else {
        free(blk->real_ptr);
    }
}

void mem_pool_dump_stats(mem_pool_ctx *mp_ctx)
{
    int total = 0;
    size_t total_size = 0;
    mem_blk *cur = NULL;

    pthread_mutex_lock(&mp_ctx->glb_lock);
    for (cur = mp_ctx->active_list; cur; cur = cur->next) {
        if (!atomic_load(&cur->freed)) {
            total++;
            total_size += cur->size;
        }
    }
    printf("[mem_pool] Active blocks: %d | Total bytes: %zu\n", total, total_size);
    pthread_mutex_unlock(&mp_ctx->glb_lock);
}

void mem_pool_dump_leaks(mem_pool_ctx *mp_ctx)
{
    int leaks = 0;
    mem_blk *cur = NULL;
    int i;

    pthread_mutex_lock(&mp_ctx->glb_lock);
    for (cur = mp_ctx->active_list; cur; cur = cur->next) {
        if (!atomic_load(&cur->freed)) {
            void* user_ptr = (void*)((char*)cur + sizeof(mem_blk));

            leaks++;
            printf("[Leak] %p size=%zu label='%s' time=%s", user_ptr, cur->size, cur->label, ctime(&cur->alloc_time));
            if (mp_ctx->debug_mode && cur->frames > 0) {
                char** symbols = backtrace_symbols(cur->callstack, cur->frames);

                for (i = 0; i < cur->frames; ++i)
                    printf("    %s\n", symbols[i]);
                free(symbols);
            }
        }
    }
    if (leaks == 0)
        printf("[mem_pool] No memory leaks detected.\n");
    pthread_mutex_unlock(&mp_ctx->glb_lock);
}
