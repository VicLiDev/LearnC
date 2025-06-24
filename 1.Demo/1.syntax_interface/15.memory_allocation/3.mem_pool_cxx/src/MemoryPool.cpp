/*************************************************************************
    > File Name: MemoryPool.cpp
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 24 Jun 2025 08:21:54 PM CST
 ************************************************************************/

#include "MemoryPool.h"

MemoryPool::MemoryPool() {
    const size_t slab_sizes[] = { 32, 64, 128, 256, 512, 1024, 2048, 4096 };
    for (size_t s : slab_sizes)
        slabs_.push_back({ s });
}

MemoryPool::~MemoryPool() {
    dumpSlabStats();
    dumpLeaks();

    for (auto& [_, blk] : active_blocks_) {
        std::free(blk->real_ptr);
        delete blk;
    }

    for (auto& slab : slabs_) {
        for (auto blk : slab.free_list) {
            std::free(blk->real_ptr);
            delete blk;
        }
    }
}

void MemoryPool::setGuard(unsigned char* ptr) {
    std::memset(ptr, GUARD_PATTERN, GUARD_SIZE);
}

bool MemoryPool::checkGuard(const unsigned char* ptr) const {
    for (int i = 0; i < GUARD_SIZE; ++i)
        if (ptr[i] != GUARD_PATTERN) return false;
    return true;
}

int MemoryPool::findSlabIndex(size_t size) const {
    for (size_t i = 0; i < slabs_.size(); ++i) {
        if (slabs_[i].chunk_size >= size)
            return static_cast<int>(i);
    }
    return -1;
}

void* MemoryPool::allocate(size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    Block* blk = nullptr;
    int slab_idx = findSlabIndex(size);

    if (slab_idx >= 0 && !slabs_[slab_idx].free_list.empty()) {
        blk = slabs_[slab_idx].free_list.front();
        slabs_[slab_idx].free_list.pop_front();
        slabs_[slab_idx].allocated_blocks++;
    } else {
        blk = new Block();
        size_t total = size + 2 * GUARD_SIZE;
        blk->real_ptr = std::malloc(total);
        if (slab_idx >= 0)
            slabs_[slab_idx].total_blocks++;
    }

    blk->size = size;
    blk->user_ptr = static_cast<unsigned char*>(blk->real_ptr) + GUARD_SIZE;
    setGuard(static_cast<unsigned char*>(blk->real_ptr));
    setGuard(static_cast<unsigned char*>(blk->user_ptr) + size);
    blk->frames = ::backtrace(blk->backtrace, MAX_BACKTRACE_DEPTH);
    active_blocks_[blk->user_ptr] = blk;

    return blk->user_ptr;
}

void MemoryPool::deallocate(void* ptr) {
    if (!ptr) return;
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = active_blocks_.find(ptr);
    if (it == active_blocks_.end()) {
        std::cerr << "Double free or invalid pointer\n";
        return;
    }

    Block* blk = it->second;

    if (!checkGuard((unsigned char*)blk->real_ptr) ||
        !checkGuard((unsigned char*)blk->user_ptr + blk->size)) {
        std::cerr << "Memory corruption detected!\n";
    }

    active_blocks_.erase(it);

    int slab_idx = findSlabIndex(blk->size);
    if (slab_idx >= 0) {
        slabs_[slab_idx].free_list.push_back(blk);
        slabs_[slab_idx].allocated_blocks--;
    } else {
        std::free(blk->real_ptr);
        delete blk;
    }
}

void MemoryPool::dumpLeaks() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& [_, blk] : active_blocks_) {
        std::cerr << "Memory leak detected: " << blk->size << " bytes\n";
        ::backtrace_symbols_fd(blk->backtrace, blk->frames, fileno(stderr));
    }
}

void MemoryPool::dumpSlabStats() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "\n[Slab Stats]\n";
    for (const auto& slab : slabs_) {
        std::cout << "Slab size " << slab.chunk_size << " bytes:\n"
                  << "  Total blocks:     " << slab.total_blocks << "\n"
                  << "  Allocated blocks: " << slab.allocated_blocks << "\n"
                  << "  Free blocks:      " << slab.free_list.size() << "\n";
    }
}

