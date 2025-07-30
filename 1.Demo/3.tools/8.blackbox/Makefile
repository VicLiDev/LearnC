
TARGET := bbox

SRCS := blackbox.c main.c
OBJS := blackbox.o main.o

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	gcc -o $(TARGET) $(OBJS)

$(OBJS): $(SRCS)

clean:
	rm -rf $(TARGET) $(OBJS)
	rm blackbox.log
