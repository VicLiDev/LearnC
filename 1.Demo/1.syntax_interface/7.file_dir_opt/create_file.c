/*************************************************************************
    > File Name: create_file.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Wed Apr 10 10:53:44 2024
 ************************************************************************/

/*
 * 文件的类型和编码方式由写入的内容决定，而不是打开/创建方式决定，即便以二进制
 * 写入模式打开文件，但是使用fprintf写入字符串，也会变成文本文件
 *
 * 需要注意的是，写入二进制文件时一般用fwrite，而写入文本文件一般使用fprintf
 *
 * 如果写入中文，编码方式会默认为Unicode text, UTF-8 text
 * 如果写入英文，或者数字字符串，编码方式会默认为 ASCII text
 */

#include <stdio.h>
#include <unistd.h>   // access
#include <sys/stat.h> // stat

int create_txt(char *file_name)
{
    FILE *file_ptr;

    // 使用写入模式打开文件，如果文件不存在则创建，如果文件存在则清空内容
    file_ptr = fopen(file_name, "w");

    if (file_ptr == NULL) {
        printf("create file fail!\n");
        return 1;
    }

    // write data to file
    fprintf(file_ptr, "这是txt文件的内容\n");

    fclose(file_ptr);

    return 0;
}

int create_ascii(char *file_name)
{
    FILE *file_ptr;

    // 使用写入模式打开文件，如果文件不存在则创建，如果文件存在则清空内容
    file_ptr = fopen(file_name, "w");

    if (file_ptr == NULL) {
        printf("create file fail!\n");
        return 1;
    }

    // write data to file
    fprintf(file_ptr, "This context of ascii file\n");

    fclose(file_ptr);

    return 0;
}

int create_bin(char *file_name)
{
    FILE *file_ptr;

    // 使用二进制写入模式打开文件，如果文件不存在则创建，如果文件存在则清空内容
    file_ptr = fopen(file_name, "wb");

    if (file_ptr == NULL) {
        printf("create file fail!\n");
        return 1;
    }

    // write data to file
    char data[] = {0x01, 0x02, 0x03, 0x04};
    fwrite(data, sizeof(char), sizeof(data), file_ptr);

    fclose(file_ptr);

    return 0;
}


int create_cvs(char *file_name)
{
    FILE *file_ptr;

    // 使用写入模式打开文件，如果文件不存在则创建，如果文件存在则追加内容
    file_ptr = fopen(file_name, "a");

    if (file_ptr == NULL) {
        printf("create file fail!\n");
        return 1;
    }

    // writ csv data to file
    fprintf(file_ptr, "Name,Age,Grade\n");
    fprintf(file_ptr, "John,25,A\n");
    fprintf(file_ptr, "Emily,30,B\n");

    fclose(file_ptr);

    return 0;
}

int check_file_exist(char* f_name)
{

    /*
     * 跨平台兼容性：
     * access() 主要在 Unix/Linux 系统可用，Windows 下需用 _access()（需 <io.h>）。
     * stat() 是跨平台的，但 Windows 和 Unix 的实现略有差异。
     * 权限问题：
     * 即使文件存在，如果程序没有权限访问目录或文件，函数也可能返回错误。
     * 符号链接：
     * stat() 会解析符号链接（检查目标文件是否存在），而 lstat() 可以检查符号链接本身是否存在。
     */


    /*
     * F_OK 是 access() 的一个标志，用于检查文件是否存在。
     * 如果返回值为 0，表示文件存在；否则（返回 -1），文件不存在或不可访问。
     *
     * 还有其他标志可以用于检查文件的访问权限。这些标志可以单独使用，也可以通过按位或 (|) 组合使用。
     * 标志  含义                   说明
     * F_OK  检查文件是否存在       仅测试文件是否存在，不涉及权限。
     * R_OK  检查读权限 (Read)      测试当前进程是否有读取文件的权限。
     * W_OK  检查写权限 (Write)     测试当前进程是否有写入文件的权限。
     * X_OK  检查执行权限 (eXecute) 测试当前进程是否有执行文件的权限（在 Unix/Linux 下对脚本或二进制文件有效）。
     */
    if (access(f_name, F_OK) == 0)
        printf("file:%s exist\n", f_name);
    else
        printf("file:%s not exist\n", f_name);

    /*
     * 如果 stat() 返回 0，表示文件存在；否则（返回 -1），文件不存在或无法访问。
     */
    struct stat buffer;

    if (stat(f_name, &buffer) == 0)
        printf("file:%s exist\n", f_name);
    else
        printf("file:%s not exist\n", f_name);

    return 0;
}

int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;
    char *f_txt = "file_text.txt";
    char *f_ascii = "file_ascii.txt";
    char *f_bin = "file_binary.bin";
    char *f_csv = "file_csv.csv";

    create_txt(f_txt);
    create_ascii(f_ascii);
    create_bin(f_bin);
    create_cvs(f_csv);

    check_file_exist(f_txt);
    check_file_exist(f_ascii);
    check_file_exist(f_bin);
    check_file_exist(f_csv);

    return 0;
}
