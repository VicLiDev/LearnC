在 Linux/Unix 系统中，使用 __attribute__((constructor)) 修饰的函数会在动态链接库
（.so 文件）被加载到进程时自动执行，具体时机如下：
* 主程序启动时：如果程序依赖该动态库（比如编译时链接或通过 LD_PRELOAD 加载），
  则这些函数会在 main() 函数执行之前被调用。
* 动态加载时：如果通过 dlopen() 显式加载动态库（且未使用 RTLD_LAZY 等延迟加载
  标志），constructor 函数会在 dlopen() 调用期间执行。
* 对应的 __attribute__((destructor)) 函数会在库卸载时（如程序退出或 dlclose() 调用）
  执行，顺序与 constructor 相反。

在 demo 中，可以修改 worker.cpp 中的宏定义 USE_DL_OPT 来观察效果



__attribute__((constructor)) 和 __attribute__((destructor)) 函数的执行是由 动态链接器
（如 Linux 下的 ld.so）实现的



动态库加载到进程中，应该是分两步来实现的，第一步是加载到ddr，第二步是映射到进程中，
那么 constructor/destructor 是与第一步强相关还是与第二步强相关呢？

| 阶段                  | 相关操作                   | 是否触发 constructor/destructor？ |
|-----------------------|----------------------------|-----------------------------------|
| 加载到物理内存（DDR） | 磁盘 → 内核缓冲区          | ❌ 无关                           |
| 映射到进程地址空间    | 虚拟地址分配、符号解析     | ✅ 直接触发 constructor           |
| 解除进程映射          | 释放虚拟地址、减少引用计数 | ✅ 直接触发 destructor            |
