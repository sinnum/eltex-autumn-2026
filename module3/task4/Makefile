CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g
TARGET  = pc_app
OBJS    = main.o shm_sync.o producer.o consumer.o

# Цель по умолчанию
all: $(TARGET)

# Линковка всех объектных файлов в один исполняемый файл
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Правило сборки объектных файлов из .c (зависит от shm_sync.h)
%.o: %.c shm_sync.h
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка проекта от объектных файлов и бинарника
clean:
	rm -f $(OBJS) $(TARGET)

# Очистка ресурсов IPC в системе (на случай аварийного завершения)
clean-ipc:
	ipcrm --all=shm
	ipcrm --all=sem

.PHONY: all clean clean-ipc