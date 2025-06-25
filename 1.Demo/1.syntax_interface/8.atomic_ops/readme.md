# gcc和libc原子操作的区别

## 🧠 核心区别：`<stdatomic.h>` vs Linux 原语

| 特点            | C11 原子（`<stdatomic.h>`） | Linux 原语（如 `__sync_*` / `__atomic_*`） |
| --------------- | --------------------------- | ------------------------------------------ |
| **归属**        | C语言标准库（从 C11 开始）  | GCC/Clang 的编译器内建特性（非标准）       |
| **平台兼容性**  | 跨平台（理论上支持所有平台）| GCC/Clang 特有，偏向 Linux/GNU 系统        |
| **头文件**      | `<stdatomic.h>`             | 无需头文件，GCC 内建函数                   |
| **移植性**      | 高（标准库）                | 较低（非标准，其他编译器不一定支持）       |
| **API 风格**    | 类型安全、语法更规范        | 函数样式，语义粗糙但直接                   |
| **控制力度**    | 可配置 memory order（顺序语义） | 一般只有强顺序（`sync`），或用 `__atomic_*` 指定 |
| **编译器要求**  | 需支持 C11                  | GCC >= 4.1（`__sync`）或 4.7（`__atomic`） |

---

## 🔧 示例对比

### 1. 使用 C11 的 `<stdatomic.h>`（推荐现代写法）

```c
#include <stdatomic.h>

atomic_int counter = 0;

void inc() {
    atomic_fetch_add(&counter, 1);
}
```

### 2. 使用 GCC 的原语 `__sync_fetch_and_add`

```c
int counter = 0;

void inc() {
    __sync_fetch_and_add(&counter, 1);
}
```

### 3. 使用更现代的 `__atomic_fetch_add`（更灵活）

```c
int counter = 0;

void inc() {
    __atomic_fetch_add(&counter, 1, __ATOMIC_SEQ_CST);  // 指定内存序
}
```


## 💬 举个类比帮助理解：

* `stdatomic.h` 相当于 **标准化、类型安全、有 memory order 的现代 C 风格接口**。
* `__sync_*` 和 `__atomic_*` 是 **GCC 提供的底层、高性能但更危险的武器**，偏工程性。


## 🧩 总结

| 项目            | C11 `<stdatomic.h>`         | GCC 原语 `__sync_*` / `__atomic_*`   |
| --------------- | --------------------------- | ------------------------------------ |
| 是 C 标准库吗？ | 是，C11 正式标准的一部分    | 否，是编译器特性（GCC 内建函数）     |
| 属于 Linux 吗？ | 不是，跨平台的 C 语言特性   | 是，GCC/Linux 平台特性（非标准）     |
| 谁更现代？      | `<stdatomic.h>`（推荐使用） | `__sync` 较老，`__atomic` 较新但复杂 |
| 谁更安全？      | `<stdatomic.h>` 类型安全    | `__sync` 不安全（原始指针，整数）    |


如果开发的是跨平台项目或者追求可维护性，建议使用 **C11 的 `<stdatomic.h>`**。
如果在写内核模块、底层驱动或强依赖 GCC 的 Linux 应用，也可以用 `__atomic_*` 或
`__sync_*`，但要注意兼容性。

