all:
	@gcc mmap_write.c -o mmap_write
	@gcc mmap_read.c -o mmap_read

test:
	@./mmap_write && ./mmap_read

clean:
	@rm ./mmap_write ./mmap_read ./mFile.txt
