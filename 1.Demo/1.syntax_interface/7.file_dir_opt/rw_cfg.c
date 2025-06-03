/*************************************************************************
    > File Name: rw_cfg.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 03 Jun 2025 09:52:25 AM CST
 ************************************************************************/

/*
 * 主要字符串操作说明
 * 
 * 1. **trim_string() 函数**，字符串修剪(去除首尾空白):
 *    - 使用`isspace()`函数检测空白字符
 *    - 采用双指针技术定位非空白字符的起始和结束位置
 *    - 使用`memmove()`函数将修剪后的字符串移动到开头
 *    - 该函数会直接修改原始字符串
 * 
 * 2. **to_lowercase() 函数**，大小写转换:
 *    - 遍历字符串中的每个字符
 *    - 使用`tolower()`将每个字符转为小写
 *    - 直接在原字符串上进行修改
 * 
 * 3. **parse_bool() 函数**，安全复制:
 *    - 使用`strncpy()`安全地创建字符串副本
 *    - 转换为小写以实现不区分大小写的比较
 *    - 使用`strcmp()`匹配多种表示"真"的字符串形式("true"/"1"/"yes")
 * 
 * 4. **配置解析过程**，字符串分割:
 *    - 使用`strchr()`定位等号(=)的位置
 *    - 通过临时插入空字符('\0')来分割字符串
 *    - 使用`strcspn()`查找换行符位置
 * 
 * 5. **交互式搜索功能**，类型转换和验证:
 *    - 使用`fgets()`安全读取用户输入
 *    - 使用`strncpy()`进行字符串复制
 *    - 使用`strtok()`进行字符串分割(分词)
 * 
 * 6. **其他重要操作**，用户输入处理:
 *    - `strlen()`获取字符串长度
 *    - `strcmp()`进行字符串比较
 *    - `strtok()`的二次调用继续分割字符串
 *    - 字符串修剪前后的边界检查
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>  // For isspace()
#include <stdbool.h>

// 写配置文件
void write_config_file(const char* filename)
{
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Failed to create config file %s\n", filename);
        return;
    }
    
    // Write configuration items
    fprintf(file, "# Sample configuration file\n");
    fprintf(file, "server_ip = 192.168.1.100\n");
    fprintf(file, "server_port = 8080\n");
    fprintf(file, "timeout = 30\n");
    fprintf(file, "debug_mode = true\n");
    fprintf(file, "username = admin\n");
    fprintf(file, "welcome_message = Hello, World!\n");  // String with spaces
    
    fclose(file);
    printf("Config file %s created successfully\n", filename);
}

// 修剪字符串两端的空格（修改原始字符串）
void trim_string(char* str)
{
    // 双指针法去除首尾空白字符
    char* start = str;

    /*
     * int isspace(int c);
     * 1. 参数说明
     *   c: 要检测的字符，以 int 形式传递
     *     必须是可以表示为 unsigned char 的值或 EOF
     *     如果传入 char 类型，应先转换为 unsigned char 以避免符号扩展问题
     * 2. 返回值
     *   非零值（true）：如果 c 是空白字符
     *   零（false）：如果 c 不是空白字符
     * 3. 判断的空白字符
     *   标准规定的空白字符包括（可能因locale不同而有所变化）：
     *     空格 (' ')
     *     换页符 ('\f')
     *     换行符 ('\n')
     *     回车符 ('\r')
     *     水平制表符 ('\t')
     *     垂直制表符 ('\v')
     */
    while (isspace((unsigned char)*start)) start++;
    
    char* end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    
    // Move trimmed string to beginning
    /*
     * memmove() 是 C 标准库中用于内存块复制的重要函数，与 memcpy() 类似但具有
     * 处理内存重叠区域的能力。
     *
     * void *memmove(void *dest, const void *src, size_t n);
     * 1. 参数说明
     *   dest: 目标内存地址（复制到此处）
     *   src: 源内存地址（从此处复制）
     *   n: 要复制的字节数
     * 2. 返回值
     *   返回目标内存地址 dest 的指针
     * 3. 核心特性
     *   3.1 内存重叠处理
     *     memmove() 的关键特点是它能正确处理源内存和目标内存重叠的情况：
     *     当 dest 在 src 之前时：从前往后复制（与 memcpy 相同）
     *     当 dest 在 src 之后时：从后往前复制（避免覆盖未复制的数据）
     *   3.2 安全保证
     *     保证在内存重叠时也能正确复制
     *     比 memcpy() 更安全但可能稍慢
     */
    memmove(str, start, end - start + 1);
    str[end - start + 1] = '\0';
}

// 将字符串转换为小写（修改原始字符串）
void to_lowercase(char* str)
{
    // 遍历字符串将每个字符转为小写
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// 从字符串解析布尔值
bool parse_bool(const char* value)
{
    // 创建临时字符串副本进行比较
    char temp[16];
    strncpy(temp, value, sizeof(temp)-1);
    temp[sizeof(temp)-1] = '\0';
    to_lowercase(temp);
    trim_string(temp);
    
    if (strcmp(temp, "true") == 0 || strcmp(temp, "1") == 0 || strcmp(temp, "yes") == 0) {
        return true;
    }
    return false;
}

// 解析配置文件
void read_config_file(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open config file %s\n", filename);
        return;
    }
    
    printf("\nReading config file %s:\n", filename);
    
    char line[256];
    int line_num = 0;
    
    /*
     * fgets 函数
     * 功能：从文件流 stream 中读取字符，直到遇到以下情况之一停止：
     *   读取到 n-1 个字符（保留1个位置给终止符 \0）
     *   遇到 换行符 \n（会包含在结果中）
     *   到达 文件末尾 (EOF)
     * 
     * 返回值：
     *   成功时返回 str（指向输入的缓冲区）
     *   失败或读到文件尾时返回 NULL
     */
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n')
            continue;
        
        /*
         * Remove trailing newline
         * 1. 函数原型 size_t strcspn(const char *str, const char *reject);
         * 2. 参数说明
         *   str: 要扫描的主字符串
         *   reject: 包含要排除的字符集合的字符串
         * 3. 返回值
         *   返回 str 开头连续不包含 reject 中任何字符的字符数（即第一个匹配到 reject 中字符的位置）。
         * 4. 工作原理
         *   strcspn() 会从 str 的开头开始扫描，逐个检查字符是否出现在 reject 中：
         *   如果当前字符不在 reject 中，计数器增加
         *   如果当前字符在 reject 中，立即停止扫描并返回当前计数值
         *   如果扫描完整个 str 都没有匹配到 reject 中的字符，返回 str 的长度
         *   reject 可以包含多个字符，每个字符都单独有效，例如：
         *   if (strcspn(username, "!@#$%^&*") == strlen(username)) {
         *      // 用户名不包含特殊字符
         *   }
         */
        line[strcspn(line, "\n")] = '\0';
        
        /*
         * char *strchr(const char *str, int c);
         * 在字符串中查找特定字符的首次出现位置
         *
         * 1. 参数说明
         *   str: 要搜索的源字符串（以 null 结尾）
         *   c: 要查找的字符（以 int 形式传递，但实际作为 char 处理）
         * 2. 返回值
         *   成功：返回指向字符串中第一次出现字符 c 的指针
         *   失败：如果字符未找到，返回 NULL 指针
         * 3. 关键特性
         *   3.1 搜索范围
         *     搜索包含字符串的 null 终止符 \0
         *     因此可以用来查找字符串结尾
         *   3.2 大小写敏感
         *     区分大小写（'A' 和 'a' 被视为不同字符）
         *     如需不区分大小写，需自行转换大小写
         */
        char* equals = strchr(line, '=');
        if (equals == NULL) {
            printf("Warning: Line %d has invalid format (missing '=')\n", line_num);
            continue;
        }
        
        // Split into key and value
        *equals = '\0';  // Temporarily terminate key
        char* key = line;
        char* value = equals + 1;
        
        // Trim whitespace from both
        trim_string(key);
        trim_string(value);
        
        // Skip empty keys
        if (strlen(key) == 0) {
            printf("Warning: Line %d has empty key\n", line_num);
            continue;
        }
        
        // Process different value types
        if (strcmp(key, "server_port") == 0 || strcmp(key, "timeout") == 0) {
            // Numeric value
            int num = atoi(value);
            printf("Config: %-15s = %d (number)\n", key, num);
        } else if (strcmp(key, "debug_mode") == 0) {
            // Boolean value
            bool b = parse_bool(value);
            printf("Config: %-15s = %s (boolean: %s)\n", key, value, b ? "true" : "false");
        } else {
            // String value
            printf("Config: %-15s = \"%s\" (string)\n", key, value);
        }
    }
    
    fclose(file);
}

// Interactive function to search for config key
void search_config(const char* filename)
{
    char search_key[100];
    printf("\nEnter config key to search: ");
    fgets(search_key, sizeof(search_key), stdin);
    
    // Trim the input
    trim_string(search_key);
    if (strlen(search_key) == 0) {
        printf("No key entered\n");
        return;
    }
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open config file %s\n", filename);
        return;
    }
    
    char line[256];
    bool found = false;
    
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n') continue;
        
        // Make a copy for strtok
        /*
         * char *strncpy(char *dest, const char *src, size_t n);
         * 1. 参数说明
         *   dest: 目标字符数组（复制到此）
         *   src: 源字符串（从此复制）
         *   n: 最多复制的字符数
         * 2. 返回值
         *   返回目标字符串 dest 的指针
         * 3. 关键特性
         *   4.1 复制行为
         *     从 src 复制最多 n 个字符到 dest
         *     如果 src 长度小于 n:
         *     复制全部 src 内容
         *     用 \0 填充剩余空间直到写入 n 个字符
         *     如果 src 长度大于等于 n:
         *     只复制前 n 个字符
         *     不会自动添加 null 终止符
         *   4.2 与 strcpy() 的区别
         *     特性       strncpy             strcpy
         *     长度限制   ✅ 有               ❌ 无
         *     自动补\0   仅当 src 短于 n     ✅ 总是
         *     填充行为   用 \0 填充剩余空间  无
         *     安全性     较高                较低
         */
        char line_copy[256];
        strncpy(line_copy, line, sizeof(line_copy));
        
        /*
         * char *strtok(char *str, const char *delim);
         * 1. 参数说明
         *   str: 要分割的字符串（第一次调用时传入，后续传入 NULL）
         *   delim: 包含分隔符的字符串（多个字符都被视为独立分隔符）
         * 2. 返回值
         *   成功：找到并分割出一个标记(token)时，它会返回一个指向这个标记的指针
         *   失败/无更多标记：返回 NULL
         * 3. 关键特性
         *   3.1 分割行为
         *     会修改原始字符串，将分隔符替换为 \0
         *     使用静态变量保存位置，因此不是线程安全的
         *     连续的分隔符被视为单个分隔符
         *   3.2 调用模式
         *     第一次调用：传入要分割的字符串
         *     后续调用：传入 NULL 继续从上次位置分割
         * 4. 注意事项
         *   4.1 重要限制
         *     修改原始字符串：会破坏原字符串内容
         *     非线程安全：使用静态变量保存状态
         *     不可重入：不能嵌套调用
         *     空标记：无法区分连续分隔符和空字段
         * 5. 可重入版本
         *   strtok_r() (可重入版本)
         */
        // char* key = strtok(line_copy, "=");
        char *saveptr; // 用于保存分割状态的指针
        char* key = strtok_r(line_copy, "=", &saveptr);
        if (key == NULL) continue;
        
        trim_string(key);
        if (strcmp(key, search_key) == 0) {
            // char* value = strtok(NULL, "=");
            char* value = strtok_r(NULL, "=", &saveptr);
            if (value != NULL) {
                trim_string(value);
                printf("Found: %s = %s\n", key, value);
                found = true;
            }
        }
    }
    
    if (!found) {
        printf("Key '%s' not found in config\n", search_key);
    }
    
    fclose(file);
}

int main()
{
    const char* config_file = "config.cfg";
    
    // Write config file
    write_config_file(config_file);
    
    // Read and display config
    read_config_file(config_file);
    
    // Interactive search
    search_config(config_file);
    
    return 0;
}
