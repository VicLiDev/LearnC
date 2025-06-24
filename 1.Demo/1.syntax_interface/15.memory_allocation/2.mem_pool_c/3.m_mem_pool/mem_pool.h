/*************************************************************************
    > File Name: mem_pool.h
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 24 Jun 2025 06:07:21 PM CST
 ************************************************************************/

#ifndef __MEM_POOL_H__
#define __MEM_POOL_H__

#include <stddef.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct mem_slab_pool_ctx_t {
    size_t slab_cls_cnt;
    slab_class slabs[MAX_SLAB_CLASSES];
    mem_blk *active_list;
    pthread_mutex_t glb_lock;
    bool debug_mode;
    bool protect_mode;
} mem_pool_ctx;

void mem_pool_init(mem_pool_ctx **mp_ctx);
void mem_pool_destroy(mem_pool_ctx *mp_ctx);
void mem_pool_set_debug(mem_pool_ctx *mp_ctx, int enabled);
void mem_pool_set_protection(mem_pool_ctx *mp_ctx, int enabled);
void* mem_pool_alloc(mem_pool_ctx *mp_ctx, size_t size);
void* mem_pool_alloc_label(mem_pool_ctx *mp_ctx, size_t size, const char* label);
void mem_pool_free(mem_pool_ctx *mp_ctx, void* ptr);
void mem_pool_dump_stats(mem_pool_ctx *mp_ctx);
void mem_pool_dump_leaks(mem_pool_ctx *mp_ctx);

#ifdef __cplusplus
}
#endif

#endif /* MEM_POOL_H__ */
