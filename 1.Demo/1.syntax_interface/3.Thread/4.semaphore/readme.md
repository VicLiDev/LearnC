在较新的Linux系统中，sem_init 及其相关的命名空间信号量（unnamed semaphores）可能
已被标记为已弃用，尤其是在与跨平台库的兼容性问题上。这通常意味着需要使用基于文件
描述符或共享内存的信号量，例如 sem_open，这在命名信号量的场景下更适合。

为了避免使用已弃用的 sem_init，我们可以用 sem_open 代替，这将允许信号量的跨进程
使用，并且适应现代的POSIX标准。

demo.c 使用 sem_init 进行编写，demo2.c使用sem_open编写

主要区别：
1. sem_open：代替 sem_init，使用命名信号量。信号量被分配了名字（例如 "/empty_slots"
   和 "/filled_slots"），这些名字是全局的，允许跨进程共享信号量。
2. sem_unlink：在程序结束时，删除信号量的名称链接，以便其他进程不再访问这些信号量。
3. sem_close：释放当前进程中的信号量资源。

semaphore.c 是之前的老demo
