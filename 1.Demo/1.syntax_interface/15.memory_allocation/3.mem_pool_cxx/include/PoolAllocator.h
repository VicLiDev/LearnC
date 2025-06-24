/*************************************************************************
    > File Name: PoolAllocator.h
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 24 Jun 2025 08:21:28 PM CST
 ************************************************************************/

#ifndef __POOLALLOCATOR_H__
#define __POOLALLOCATOR_H__

#include "MemoryPool.h"

template <typename T>
class PoolAllocator {
public:
    using value_type = T;

    PoolAllocator() noexcept : pool_(getGlobalPool()) {}
    PoolAllocator(const PoolAllocator&) noexcept = default;

    template <typename U>
    PoolAllocator(const PoolAllocator<U>&) noexcept : pool_(getGlobalPool()) {}

    T* allocate(std::size_t n) {
        return static_cast<T*>(pool_->allocate(n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t) {
        pool_->deallocate(p);
    }

    template <typename U>
    struct rebind {
        using other = PoolAllocator<U>;
    };

    static MemoryPool* getGlobalPool() {
        static MemoryPool global_pool;
        return &global_pool;
    }

private:
    MemoryPool* pool_;
};

template <typename T, typename U>
inline bool operator==(const PoolAllocator<T>&, const PoolAllocator<U>&) { return true; }
template <typename T, typename U>
inline bool operator!=(const PoolAllocator<T>&, const PoolAllocator<U>&) { return false; }

#endif /* POOLALLOCATOR_H__ */
