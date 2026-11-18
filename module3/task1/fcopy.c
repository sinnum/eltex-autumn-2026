#include "fcopy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

static void child_process(int fd_read_data, int fd_write_sync);
static void parent_process(int fd_write_data, int fd_read_sync, char *filenames[], int file_count);

void start_file_copy(char *filenames[], int file_count, const char *pipe_name) {
    int data_pipe[2]; // parent -> child
    int sync_pipe[2]; // child -> parent
    char name_data[512] = {0};
    char name_sync[512] = {0};

    int is_named = (pipe_name != NULL);

    if (is_named) {
        // если указан именованный создаем 2 fifo в tmp
        snprintf(name_data, sizeof(name_data), "%s_data", pipe_name);
        snprintf(name_sync, sizeof(name_sync), "%s_sync", pipe_name);

        unlink(name_data);
        unlink(name_sync);

        if (mkfifo(name_data, 0666) == -1 || mkfifo(name_sync, 0666) == -1) {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }

        // O_RDWR чтобы open не блокировался
        data_pipe[0] = data_pipe[1] = open(name_data, O_RDWR);
        sync_pipe[0] = sync_pipe[1] = open(name_sync, O_RDWR);

        if (data_pipe[0] == -1 || sync_pipe[0] == -1) {
            perror("open fifo");
            exit(EXIT_FAILURE);
        }
    } else {
        // неименованные каналы
        if (pipe(data_pipe) == -1 || pipe(sync_pipe) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

        pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {        snprintf(name_data, sizeof(name_data), "%s_data", pipe_name);
        snprintf(name_sync, sizeof(name_sync), "%s_sync", pipe_name);
        // дочерний процесс
        
        
        if (!is_named) {
            close(data_pipe[1]); 
            close(sync_pipe[0]); 
        }
        
        child_process(data_pipe[0], sync_pipe[1]);
        
        if (!is_named) {
            close(data_pipe[0]);
            close(sync_pipe[1]);
        }
        exit(EXIT_SUCCESS);
    } else {
        // родительский процесс
        
        if (!is_named) {
            close(data_pipe[0]); 
            close(sync_pipe[1]); 
        }

        parent_process(data_pipe[1], sync_pipe[0], filenames, file_count);

        if (!is_named) {
            close(data_pipe[1]);
            close(sync_pipe[0]);
        }
        
        wait(NULL); // Ожидаем завершения

        if (is_named) {
            unlink(name_data);
            unlink(name_sync);
        }
    }
}


static void child_process(int fd_read_data, int fd_write_sync) {
    char buf[BUFFER_SIZE];
    struct file_header header;
    char ready_signal = 'R';

    while (1) {
        // Отправляем родителю сигнал готовности
        if (write(fd_write_sync, &ready_signal, 1) != 1) break;

        // Читаем заголовок файла
        ssize_t n = read(fd_read_data, &header, sizeof(header));
        if (n != sizeof(header)) break;

        // Если размер -1, родитель дал команду завершения
        if (header.size == -1) break;
        // Если 0, значит файл не найден (родитель уже вывел ошибку)
        if (header.size == 0) continue;

        // Формируем имя выходного файла
        char out_name[512];
        snprintf(out_name, sizeof(out_name), "%s.copy", header.filename);

        // Открываем файл на запись (создаем или обнуляем)
        int out_fd = open(out_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out_fd == -1) {
            perror("open out");
            continue;
        }

        // Читаем данные из канала, пока не получим ровно size байт
        long long remaining = header.size;
        while (remaining > 0) {
            int chunk = (remaining > BUFFER_SIZE) ? BUFFER_SIZE : remaining;
            ssize_t bytes_read = read(fd_read_data, buf, chunk);
            if (bytes_read <= 0) break; // Ошибка или конец файла

            write(out_fd, buf, bytes_read);
            remaining -= bytes_read;
        }
        close(out_fd);
    }
}

static void parent_process(int fd_write_data, int fd_read_sync, char *filenames[], int file_count) {
    char buf[BUFFER_SIZE];
    struct file_header header;
    char ready_signal;

    for (int i = 0; i < file_count; i++) {
        // Ждем, пока потомок сообщит о готовности
        if (read(fd_read_sync, &ready_signal, 1) != 1) break;

        struct stat st;
        // Проверяем, существует ли файл
        if (stat(filenames[i], &st) == -1) {
            fprintf(stderr, "Ошибка: файл '%s' не существует.\n", filenames[i]);
            header.size = 0; // Сообщаем потомку, что файла нет
            write(fd_write_data, &header, sizeof(header));
            continue;
        }

        // Открываем файл на чтение
        int in_fd = open(filenames[i], O_RDONLY);
        if (in_fd == -1) {
            fprintf(stderr, "Ошибка: не удалось открыть '%s'.\n", filenames[i]);
            header.size = 0;
            write(fd_write_data, &header, sizeof(header));
            continue;
        }

        // Отправляем заголовок (имя и размер)
        strncpy(header.filename, filenames[i], sizeof(header.filename) - 1);
        header.size = st.st_size;
        write(fd_write_data, &header, sizeof(header));

        // Читаем файл и пишем в канал
        ssize_t bytes_read;
        while ((bytes_read = read(in_fd, buf, BUFFER_SIZE)) > 0) {
            write(fd_write_data, buf, bytes_read);
        }
        close(in_fd);
    }

    // Даем потомку сигнал, что работы больше нет
    read(fd_read_sync, &ready_signal, 1);
    header.size = -1;
    write(fd_write_data, &header, sizeof(header));
}
