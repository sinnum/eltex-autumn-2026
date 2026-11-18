#include "fcopy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s [-p имя_канала] файл1 [файл2 ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *pipe_name = NULL;
    char *filenames[128];
    int file_count = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) {
                pipe_name = argv[++i];
            } else {
                fprintf(stderr, "Ошибка: после -p нужно указать имя канала.\n");
                return EXIT_FAILURE;
            }
        } else {
            filenames[file_count++] = argv[i];
        }
    }

    if (file_count == 0) {
        fprintf(stderr, "Ошибка: не указаны файлы для копирования.\n");
        return EXIT_FAILURE;
    }

    start_file_copy(filenames, file_count, pipe_name);

    return EXIT_SUCCESS;
}
