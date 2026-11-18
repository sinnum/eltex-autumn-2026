CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g
TARGET  = chat
OBJS    = main.o chat.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lrt

main.o: main.c chat.h
	$(CC) $(CFLAGS) -c main.c -o main.o

chat.o: chat.c chat.h
	$(CC) $(CFLAGS) -c chat.c -o chat.o

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean