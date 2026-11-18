#include <stdio.h>
#include <string.h>
#include "msg_system.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование:\n");
        fprintf(stderr, "  %s -b                 (Брокер)\n", argv[0]);
        fprintf(stderr, "  %s -p <topic>         (Издатель)\n", argv[0]);
        fprintf(stderr, "  %s -s <topic1> ...    (Подписчик)\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-b") == 0) {
        run_broker();
    } 
    else if (strcmp(argv[1], "-p") == 0 && argc == 3) {
        run_publisher(argv[2]);
    } 
    else if (strcmp(argv[1], "-s") == 0 && argc > 2) {
        run_subscriber(argc - 2, &argv[2]);
    } 
    else {
        fprintf(stderr, "Неверные аргументы.\n");
        return 1;
    }

    return 0;
}
