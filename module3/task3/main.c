#include <stdio.h>
#include "chat.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <имя_комнаты>\n", argv[0]);
        fprintf(stderr, "Пример: %s mychat\n", argv[0]);
        return 1;
    }

    chat_init(argv[1]);
    
    chat_run();

    return 0;
}