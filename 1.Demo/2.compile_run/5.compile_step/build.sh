#!/usr/bin/env bash
#########################################################################
# File Name: build.sh
# Author: LiHongjin
# mail: 872648180@qq.com
# Created Time: Tue 22 Jul 2025 02:32:01 PM CST
#########################################################################

# ==> 基本编译步骤
#   预编译
#     将.c 文件转化成 .i文件
#     使用的gcc命令是：gcc -E
#     对应于预处理命令cpp
#   编译
#     将.c/.h文件转换成.s文件
#     使用的gcc命令是：gcc -S
#     对应于编译命令 cc -S
#   汇编
#     将.s 文件转化成 .o文件
#     使用的gcc 命令是：gcc -c
#     对应于汇编命令是 as
#   链接
#     将.o文件转化成可执行程序
#     对应于链接命令是 ld
#     使用的gcc 命令是： gcc
#
# 总结起来编译过程就上面的四个过程：
# --> 预编译处理(.c)
# --> 编译、优化程序(.s、.asm)
# --> 汇编程序(.obj、.o、.a、.ko)
# --> 链接程序(.exe、.elf、.axf等)


# ==> 实践
# 1. 预处理：生成 `.i` 文件（宏展开 + 头文件展开）
#    查看 `main.i` 内容会看到 `#include <stdio.h>` 被替换成了完整的标准库实现，
#    同时 `SQUARE(a)` 被展开为 `((a) * (a))`。
gcc -E main.c -o main.i

# 2. 编译：生成 `.s` 汇编文件（优化、转成汇编指令）
#    可以打开 `main.s` 查看生成的汇编代码，比如 x86-64 指令集中的 `mov`, `call`, `lea`, 等操作。
gcc -S main.i -o main.s

# 3. 汇编：生成 `.o` 目标文件（机器码）
#    `main.o` 是二进制格式，不能直接查看，但可以用 `objdump` 查看反汇编内容：
gcc -c main.s -o main.o
objdump -d main.o

# 4. 链接：生成最终可执行文件（.elf / .exe / .out）
gcc main.o -o main.out

# 整个流程简图
# ```txt
# main.c  ──>  main.i  ──>  main.s  ──>  main.o  ──>  main.out
#   |         |           |           |           |
# 源代码    预处理代码   汇编代码    目标文件     可执行程序
# (gcc -E)  (gcc -S)     (gcc -c)    (链接器)     (可以运行)
# ```
