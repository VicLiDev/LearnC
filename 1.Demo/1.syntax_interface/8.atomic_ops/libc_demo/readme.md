## 🔧 C语言中的原子操作（基于 C11 标准）

C11 引入了 `<stdatomic.h>` 头文件，用于支持原子操作。

### ✅ 基本原子类型

```c
#include <stdatomic.h>

atomic_int a;           // 原子整型
atomic_bool b;          // 原子布尔型
atomic_long l;          // 原子长整型
atomic_intptr_t ptr;    // 原子指针（跨平台安全）
```

### ✅ 常用的原子操作函数/宏

| 操作                  | 示例代码                              | 说明          |
| --------------------- | ------------------------------------- | ------------- |
| 原子赋值              | `atomic_store(&a, 10);`               | 设置值        |
| 原子读取              | `int x = atomic_load(&a);`            | 获取值        |
| 原子加法              | `atomic_fetch_add(&a, 1);`            | 原子加法，返回旧值 |
| 原子减法              | `atomic_fetch_sub(&a, 1);`            | 原子减法，返回旧值 |
| 原子交换              | `atomic_exchange(&a, 100);`           | 将值换成 100，返回旧值 |
| 原子比较并交换（CAS） | `atomic_compare_exchange_strong(...)` | 条件匹配则更新 |


## ✅ 总结

这个 demo 包含了以下 C 语言的原子操作：

1. 原子整型加法：`atomic_fetch_add`
2. 原子布尔 CAS：`atomic_compare_exchange_strong`
3. 原子指针交换：`atomic_exchange`
4. 原子读：`atomic_load`

