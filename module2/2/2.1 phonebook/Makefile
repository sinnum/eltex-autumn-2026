CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g
TARGET  = phonebook
OBJS    = main.o phonebook.o

# Цель по умолчанию
all: $(TARGET)

# Линковка объектных файлов в исполняемый файл
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Компиляция main.c
main.o: main.c phonebook.h
	$(CC) $(CFLAGS) -c main.c -o main.o

# Компиляция phonebook.c
phonebook.o: phonebook.c phonebook.h
	$(CC) $(CFLAGS) -c phonebook.c -o phonebook.o

# Очистка проекта от скомпилированных файлов
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean