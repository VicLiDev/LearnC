/*************************************************************************
    > File Name: mem_pool.h
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 24 Jun 2025 06:07:21 PM CST
 ************************************************************************/

#ifndef __MEM_POOL_H__
#define __MEM_POOL_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void mem_pool_set_debug(int enabled);
void mem_pool_set_protection(int enabled);
void* mem_pool_alloc(size_t size);
void* mem_pool_alloc_label(size_t size, const char* label);
void mem_pool_free(void* ptr);
void mem_pool_dump_stats();
void mem_pool_dump_leaks();

#ifdef __cplusplus
}
#endif

#endif /* MEM_POOL_H__ */
