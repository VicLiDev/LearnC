/*************************************************************************
    > File Name: 3.segregated_fits.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 24 Jun 2025 05:24:59 PM CST
 ************************************************************************/

/*
 * 分离适应（Segregated Fits）分配器
 *
 * 核心思想：
 * 将空闲块按大小分类，每类一个链表（例如：<=32B，<=64B，<=128B，...），
 * 从对应大小的链表中查找空闲块，避免线性遍历。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POOL_SIZE 1024 * 32
#define ALIGN4(x) (((x) + 3) & ~3)

#define NUM_CLASSES 5
const size_t class_limits[NUM_CLASSES] = {32, 64, 128, 256, 1024};

typedef struct Block {
    size_t size;
    int free;
    struct Block* next;
    struct Block* prev;
} Block;

char* memory_pool;
Block* class_bins[NUM_CLASSES];

void init_memory()
{
    memory_pool = (char*)malloc(POOL_SIZE);
    memset(class_bins, 0, sizeof(class_bins));

    Block* init_block = (Block*)memory_pool;
    init_block->size = POOL_SIZE - sizeof(Block);
    init_block->free = 1;
    init_block->next = NULL;
    init_block->prev = NULL;

    int class_id = NUM_CLASSES - 1;  // 放到最大桶里
    class_bins[class_id] = init_block;
}

// 返回块所属的分类桶
int get_class_id(size_t size)
{
    for (int i = 0; i < NUM_CLASSES; i++) {
        if (size <= class_limits[i]) return i;
    }
    return NUM_CLASSES - 1;
}

void add_to_class(Block* block)
{
    int class_id = get_class_id(block->size);
    block->next = class_bins[class_id];
    if (class_bins[class_id])
        class_bins[class_id]->prev = block;
    block->prev = NULL;
    class_bins[class_id] = block;
}

void remove_from_class(Block* block)
{
    int class_id = get_class_id(block->size);
    if (block->prev) block->prev->next = block->next;
    if (block->next) block->next->prev = block->prev;
    if (block == class_bins[class_id]) class_bins[class_id] = block->next;
}

void* my_malloc(size_t size)
{
    size = ALIGN4(size);
    int class_id = get_class_id(size);

    for (int i = class_id; i < NUM_CLASSES; i++) {
        Block* curr = class_bins[i];
        while (curr) {
            if (curr->free && curr->size >= size) {
                size_t remaining = curr->size - size;
                if (remaining > sizeof(Block) + 4) {
                    Block* new_block = (Block*)((char*)curr + sizeof(Block) + size);
                    new_block->size = remaining - sizeof(Block);
                    new_block->free = 1;
                    new_block->next = NULL;
                    new_block->prev = NULL;
                    add_to_class(new_block);

                    curr->size = size;
                }

                curr->free = 0;
                remove_from_class(curr);
                return (void*)((char*)curr + sizeof(Block));
            }
            curr = curr->next;
        }
    }

    return NULL;
}

void my_free(void* ptr)
{
    if (!ptr) return;
    Block* block = (Block*)((char*)ptr - sizeof(Block));
    block->free = 1;
    add_to_class(block);
}

void print_state()
{
    printf("\n[分离适应分配器状态]\n");
    for (int i = 0; i < NUM_CLASSES; i++) {
        printf("桶 %d (<= %zuB): ", i, class_limits[i]);
        Block* curr = class_bins[i];
        while (curr) {
            printf("[size=%zu] ", curr->size);
            curr = curr->next;
        }
        printf("\n");
    }
}

int main()
{
    init_memory();

    void* a = my_malloc(30);
    void* b = my_malloc(70);
    void* c = my_malloc(100);
    print_state();

    my_free(b);
    print_state();

    my_free(a);
    my_free(c);
    print_state();

    free(memory_pool);
    return 0;
}

