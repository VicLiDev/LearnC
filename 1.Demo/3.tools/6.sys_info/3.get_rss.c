/*************************************************************************
    > File Name: 3.get_rss.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Fri 20 Sep 14:36:46 2024
 ************************************************************************/

/*
 * 在Linux上，可以通过读取/proc/[pid]/statm文件来获取内存使用信息，其中RSS信息
 * 位于第二列（以页面为单位，每页通常是4KB）。
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

long get_rss(pid_t pid)
{
    char path[64];
    FILE *fp;
    long rss;

    snprintf(path, sizeof(path), "/proc/%ld/statm", (long)pid);
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Failed to open /proc/[pid]/statm");
        return -1;
    }

    if (fscanf(fp, "%*d %ld", &rss) != 1) {
        perror("Failed to read RSS from /proc/[pid]/statm");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    // Convert pages to kilobytes
    return rss * getpagesize() / 1024;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    pid_t pid = getpid();
    printf("RSS of this process: %ld KB\n", get_rss(pid));
    return 0;
}

