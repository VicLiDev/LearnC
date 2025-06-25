SRCs = main.c mem_pool.c
TARGET = mempool_demo

$(TARGET): $(SRCs)
	gcc $(SRCs) -lpthread -o $(TARGET)

test:
	valgrind --leak-check=full ./$(TARGET)

clean:
	rm $(TARGET)
