
1.base: 基础的 slab 内存管理算法，内存块描述结构体和内存buffer分开管理

Block 具备链表属性，被申请走使用的buffer放在 active_blocks 中，
空闲的按照size，分别放在 Slab_n 中
Block 和具体的buffer，在内存结构上是两块buffer

Block active_blocks

Slab_1 --> free_list(Block)
Slab_2 --> free_list(Block)
Slab_3 --> free_list(Block)



2.upgrade: 升级版本的内存池，内存块描述结构体和内存buffer放在一起管理

结构类似1.base中的写法，MemBlock具备链表属性，被申请走使用的buffer放在 active_list 中，
空闲的按照size，分别放在 Slab_n 中
不过这里的buffer管理方式有差异，MemBlock和具体的buffer，在内存结构上是同一块buffer

MemBlock* active_list

SlabClass_1 --> free_list(MemBlock)
SlabClass_2 --> free_list(MemBlock)
SlabClass_3 --> free_list(MemBlock)



3.m_mem_pool: 我自己写的，基于2.upgrade，进行改造封装
