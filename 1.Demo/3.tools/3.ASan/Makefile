
all:
	# 使用g++编译并链接ASan
	g++ -g -fsanitize=address -static-libasan -fno-omit-frame-pointer -o asan_demo asan_demo.c

test:
	./asan_demo
