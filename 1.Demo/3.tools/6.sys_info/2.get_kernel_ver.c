/*************************************************************************
    > File Name: 2.get_kernel_ver.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Fri 19 Jul 2024 12:30:42 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <sys/utsname.h>

int main(void)
{

    struct utsname buffer;
    char *p;
    long ver[16];
    int i=0;

    errno = 0;
    if (uname(&buffer) != 0) {
        perror("uname");
        exit(EXIT_FAILURE);
    }

    printf("system name = %s\n", buffer.sysname);
    printf("node name   = %s\n", buffer.nodename);
    printf("release     = %s\n", buffer.release);
    printf("version     = %s\n", buffer.version);
    printf("machine     = %s\n", buffer.machine);

#ifdef _GNU_SOURCE
    printf("domain name = %s\n", buffer.domainname);
#endif

    p = buffer.release;

    while (*p) {
        if (isdigit(*p)) { // 判断所传的字符是否是十进制数字字符
            ver[i] = strtol(p, &p, 10); // 字符串转long int
            // 第二个参数设置为str数值的下一个字符
            // 例如“12abc”解析完之后指向“a”返回12
            i++;
        } else {
            p++;
        }
    }

    printf("Kernel %ld Major %ld Minor %ld Patch %ld\n", ver[0], ver[1], ver[2], ver[3]);

    return EXIT_SUCCESS;
}
