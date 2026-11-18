CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g
TARGET  = file_copy
OBJS    = main.o fcopy.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c fcopy.h
	$(CC) $(CFLAGS) -c main.c -o main.o

fcopy.o: fcopy.c fcopy.h
	$(CC) $(CFLAGS) -c fcopy.c -o fcopy.o

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
