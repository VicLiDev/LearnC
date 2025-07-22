/*************************************************************************
    > File Name: main.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 22 Jul 2025 02:34:45 PM CST
 ************************************************************************/

#include <stdio.h>

#define SQUARE(x) ((x) * (x))

struct __kfifo {
	unsigned int	in;
	unsigned int	out;
	unsigned int	mask;
	unsigned int	esize;
	void		*data;
};

#define __STRUCT_KFIFO_COMMON(datatype, recsize, ptrtype) \
	union { \
		struct __kfifo	kfifo; \
		datatype	*type; \
		const datatype	*const_type; \
		char		(*rectype)[recsize]; \
		ptrtype		*ptr; \
		ptrtype const	*ptr_const; \
	}

#define __STRUCT_KFIFO(type, size, recsize, ptrtype) \
{ \
	__STRUCT_KFIFO_COMMON(type, recsize, ptrtype); \
	type		buf[((size < 2) || (size & (size - 1))) ? -1 : size]; \
}

#define STRUCT_KFIFO(type, size) \
	struct __STRUCT_KFIFO(type, size, 0, type)

#define DECLARE_KFIFO(fifo, type, size)	STRUCT_KFIFO(type, size) fifo


int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    DECLARE_KFIFO(mfifo, int, 32);
    int a = 5;

    printf("Square of %d is %d\n", a, SQUARE(a));
    return 0;
}

