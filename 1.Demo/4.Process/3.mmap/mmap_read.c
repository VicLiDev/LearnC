#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE_LENGTH 0x100
#define FILE_NAME "./mFile.txt"

int main (int argc, char* const argv[])
{
    (void)argc;
    (void)argv;
	int fd;
	void* file_memory;
	int integer;

	/* 打开文件 */
	fd = open(FILE_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	/* 创建映射内存 */
	file_memory = mmap (0, FILE_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close (fd);
	/* 读取整数,输出,然后将其倍增 */
	sscanf (file_memory, "%d", &integer);
	printf ("value: %d\n", integer);
	sprintf ((char*) file_memory, "%d\n", 2 * integer);
	/* 释放内存(非必须,因为程序就此结束)*/
	munmap (file_memory, FILE_LENGTH);
	return 0;
}
