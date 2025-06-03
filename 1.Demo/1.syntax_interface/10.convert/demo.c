/*************************************************************************
    > File Name: demo.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 03 Jun 2025 04:13:04 PM CST
 ************************************************************************/

/*
 * | 函数        | 用途                   | 特点与注意事项                                         |
 * | ----------- | ---------------------- | ------------------------------------------------------ |
 * | `atoi()`    | 字符串转 int           | 无法检测错误，遇到非法字符会返回 0                     |
 * | `atol()`    | 字符串转 long          | 同上                                                   |
 * | `strtod()`  | 字符串转 double        | 支持浮点，能检测非法字符并通过 `endptr` 指出位置       |
 * | `strtol()`  | 字符串转 long          | 支持指定进制，并能错误检测（`endptr`）                 |
 * | `strtoul()` | 字符串转 unsigned long | 同 `strtol`，但用于无符号数，支持十六进制（如 "0x1A"） |
 */

#include <stdio.h>
#include <stdlib.h>  // For atoi, atol, strtod, strtol, strtoul
#include <string.h>  // For strlen

int main()
{
    const char *int_str = "12345";
    const char *long_str = "9876543210";
    const char *double_str = "3.14159abc";
    const char *invalid_str = "abc123";
    const char *hex_str = "0x1A3F";

    // atoi: Convert string to int (undefined behavior on invalid input)
    int int_val = atoi(int_str);
    printf("str to int (atoi): %s --> %d\n", int_str, int_val);

    // atol: Convert string to long (undefined behavior on invalid input)
    long long_val = atol(long_str);
    printf("str to long (atol): %s --> %ld\n", long_str, long_val);

    // strtod: Convert string to double, stops at first non-numeric character
    char *endptr1;
    double double_val = strtod(double_str, &endptr1);
    printf("str to double (strtod): %s --> %f, stopped at: '%s'\n", double_str, double_val, endptr1);

    // strtol: Convert string to long with error checking
    char *endptr2;
    long strtol_val = strtol(invalid_str, &endptr2, 10);  // base 10
    printf("str to long (strtol): %s --> %ld, stopped at: '%s'\n", invalid_str, strtol_val, endptr2);

    // strtoul: Convert string to unsigned long with base 16 (hex)
    char *endptr3;
    unsigned long hex_val = strtoul(hex_str, &endptr3, 0); // base 0 = auto-detect (0x for hex)
    printf("str to unsigned long (strtoul): %s --> %lu, stopped at: '%s'\n", hex_str, hex_val, endptr3);

    return 0;
}

