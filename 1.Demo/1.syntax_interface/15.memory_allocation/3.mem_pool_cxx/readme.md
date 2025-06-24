## 📦 框架结构一览

```
memory_pool/
├── include/                // 公共头文件
│   ├── MemoryPool.h        // 内存池类声明
│   └── PoolAllocator.h     // STL allocator 封装模板
├── src/                    // 实现文件
│   └── MemoryPool.cpp      // 内存池功能实现
├── test/
│   └── test.cpp            // 示例代码：演示 vector/map 使用池分配器
├── CMakeLists.txt          // 项目构建配置
```

---

## 🎯 模块讲解

### 1. `MemoryPool` 类（核心内存池）

位于 `MemoryPool.h/cpp`，是项目的核心，提供：

| 功能项             | 描述                                                        |
| ------------------ | ----------------------------------------------------------- |
| **线程安全**           | 使用 `std::mutex` 保证多线程访问安全                        |
| **slab 分配器支持**    | 按块大小划分多级 slab（如 32/64/128/...）提升小对象分配效率 |
| **guard 区检测**       | 在分配块前后添加保护区，检测越界访问                        |
| **内存泄漏追踪**       | 每个分配记录调用栈，释放时比对，析构时自动打印未释放的块    |
| **slab 统计信息**      | 程序退出时输出每类 slab 的总块数 / 已分配 / 空闲            |
| **大块直接 malloc**    | 大于最大 slab 的内存直接 malloc/free                        |

### 2. `PoolAllocator<T>` 类（STL 分配器）

位于 `PoolAllocator.h`，是一个泛型分配器模板，可用于 STL 容器：

| 功能项                     | 描述                                         |
| -------------------------- | -------------------------------------------- |
| 泛型支持                   | 支持所有 STL 容器中元素类型                  |
| 自动对齐与 sizeof 管理     | 按元素大小自动计算分配大小                   |
| **共享全局池实例**             | 所有 allocator 实例使用同一个全局 MemoryPool |
| 可用于 `vector`, `map`, 等 | 示例中用于 `std::vector<std::string>` 和 `std::unordered_map<int, std::string>` |

### 3. 示例程序（`test.cpp`）

展示如何使用 `PoolAllocator<T>` 替代默认 STL 分配器：

* `std::vector<std::string, PoolAllocator<std::string>>`
* `std::unordered_map<int, std::string, ..., PoolAllocator<...>>`

程序退出时自动输出：

* 每个 slab 的分配统计
* 内存泄漏记录（包括调用栈）

---

## ✨ 特性亮点总结

| 类别       | 功能                                            |
| -------- | --------------------------------------------- |
| 🧱 内存模型  | 支持 slab 分配器、小对象复用、大对象直接 malloc                |
| 🧵 线程安全  | 所有操作加锁保护，适合并发场景                               |
| 🧪 调试功能  | 检测越界写入（保护区）、追踪调用栈、自动泄漏报告                      |
| 📊 分配统计  | 每类 slab：总块数、当前已分配、空闲块，帮助分析性能瓶颈                |
| 📚 STL兼容 | 可作为 allocator 插件接入所有标准容器（如 vector、list、map 等） |
| 🛠️ 构建支持 | 标准 `CMakeLists.txt`，跨平台可编译                    |
| 🧹 自动清理  | 所有分配在 `MemoryPool` 析构时清理，避免资源泄漏               |

---

## 📌 示例输出示意（正常退出）

```
Vector item #0
...
MapVal#9

[Slab Stats]
Slab size 32 bytes:
  Total blocks:     0
  Allocated blocks: 0
  Free blocks:      0
...
Slab size 256 bytes:
  Total blocks:     20
  Allocated blocks: 0
  Free blocks:      20
...
```

---

## 🧩 可扩展性建议

如果你打算继续开发，这些方向是有价值的：

| 方向       | 内容                                          |
| -------- | ------------------------------------------- |
| 🚀 性能优化  | slab 空间页分配、cacheline 对齐、无锁栈（lock-free slab） |
| 🔐 多线程扩展 | 每线程独立池，减少锁竞争                                |
| 📦 可配置参数 | 自定义 slab 层级、最大大小                            |
| 📁 导出为库  | 支持构建 `.so` 或 `.a`，安装到系统路径                   |
| 🧪 单元测试  | 引入 GoogleTest，覆盖各种使用场景                      |

