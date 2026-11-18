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
    printf("\n[Подписчик %d] Получен SIGINT. Выхожу...\n", getpid());
    exit(0);
}

void run_subscriber(int topic_count, char *topics[]) {
    int msqid = msgget(MSG_KEY, 0666);
    if (msqid == -1) {
        perror("Очередь не найдена (Брокер не запущен)");
        exit(1);
    }
    signal(SIGINT, client_sigint_handler);
    pid_t mypid = getpid();
    printf("[Подписчик %d] Запущен. Ожидание сообщений...\n", mypid);

    // Отправляем подписки брокеру
    struct msgbuf msg;
    msg.mtype = 1;
    for (int i = 0; i < topic_count; i++) {
        snprintf(msg.mtext, MAX_TEXT, "subscribe,%d,%s", mypid, topics[i]);
        msgsnd(msqid, &msg, strlen(msg.mtext) + 1, 0);
    }

    // Читаем только то, что адресовано нам mtype == mypid
    while (1) {
        if (msgrcv(msqid, &msg, MAX_TEXT, mypid, 0) == -1) {
            perror("msgrcv");
            break;
        }
        printf("\n[Получено]: %s\n> ", msg.mtext);
        fflush(stdout);
    }
}
