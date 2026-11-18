CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g
TARGET  = msg_system
OBJS    = main.o broker.o publisher.o subscriber.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c msg_system.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
