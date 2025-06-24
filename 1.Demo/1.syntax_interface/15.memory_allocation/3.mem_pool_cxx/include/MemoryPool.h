/*************************************************************************
    > File Name: MemoryPool.h
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 24 Jun 2025 08:21:07 PM CST
 ************************************************************************/

#ifndef __MEMORYPOOL_H__
#define __MEMORYPOOL_H__

#include <cstddef>
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <list>
#include <mutex>
#include <execinfo.h>
#include <iostream>
#include <memory>
#include <cstring>

#define GUARD_PATTERN 0xAB
#define GUARD_SIZE 16
#define MAX_BACKTRACE_DEPTH 10

class MemoryPool {
public:
    MemoryPool();
    ~MemoryPool();

    void* allocate(size_t size);
    void deallocate(void* ptr);
    void dumpLeaks();
    void dumpSlabStats();

private:
    struct Block {
        void* real_ptr;
        void* user_ptr;
        size_t size;
        void* backtrace[MAX_BACKTRACE_DEPTH];
        int frames;
    };

    struct Slab {
        size_t chunk_size;
        std::list<Block*> free_list;
        size_t total_blocks = 0;
        size_t allocated_blocks = 0;
    };

    std::mutex mutex_;
    std::vector<Slab> slabs_;
    std::unordered_map<void*, Block*> active_blocks_;

    int findSlabIndex(size_t size) const;
    void setGuard(unsigned char* ptr);
    bool checkGuard(const unsigned char* ptr) const;
};

#endif /* MEMORYPOOL_H__ */
