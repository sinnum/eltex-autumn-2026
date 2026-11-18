#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg_system.h"

static void client_sigint_handler(int sig) {
    (void)sig;
    printf("\n[Издатель %d] Получен SIGINT. Выхожу...\n", getpid());
    exit(0);
}

void run_publisher(const char *topic) {
    int msqid = msgget(MSG_KEY, 0666);
    if (msqid == -1) {
        perror("Очередь не найдена (Брокер не запущен)");
        exit(1);
    }
    signal(SIGINT, client_sigint_handler);
    printf("[Издатель %d] Запущен для темы '%s'. Вводите сообщения:\n", getpid(), topic);

    char input[MAX_TEXT];
    struct msgbuf msg;
    
    while (1) {
        printf("> ");
        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0;
        if (strlen(input) == 0) continue;

        msg.mtype = 1;
        snprintf(msg.mtext, MAX_TEXT, "send,%d,%s %s", getpid(), topic, input);
        
        if (msgsnd(msqid, &msg, strlen(msg.mtext) + 1, 0) == -1) {
            perror("msgsnd");
            break;
        }
    }
}
