#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg_system.h"

#define MAX_SUBS 50
#define MAX_TOPICS 10
#define MAX_PUBS 50

// Локальные структуры данных Брокера
struct subscriber {
    pid_t pid;
    int topic_count;
    char topics[MAX_TOPICS][64];
} subscribers[MAX_SUBS];
int sub_count = 0;

pid_t pubs_pids[MAX_PUBS];
int pub_count = 0;

int msqid_global = -1;

// Обработчик SIGINT
static void broker_sigint_handler(int sig) {
    (void)sig; // Заглушка для неиспользуемого параметра
    printf("\n[Брокер] Получен SIGINT. Завершаю работу...\n");
    
    // Рассылаем сигнал завершения всем, кого мы знаем
    for (int i = 0; i < sub_count; i++) {
        kill(subscribers[i].pid, SIGINT);
    }
    for (int i = 0; i < pub_count; i++) {
        kill(pubs_pids[i], SIGINT);
    }

    // Удаляем очередь из системы
    if (msqid_global != -1) {
        msgctl(msqid_global, IPC_RMID, NULL);
        printf("[Брокер] Очередь сообщений удалена.\n");
    }
    exit(0);
}

void run_broker(void) {
    // Создаем очередь с эксклюзивным флагом
    msqid_global = msgget(MSG_KEY, IPC_CREAT | IPC_EXCL | 0666);
    if (msqid_global == -1) {
        perror("Очередь уже существует или ошибка msgget");
        exit(1);
    }
    printf("[Брокер] Запущен. Очередь создана (ID: %d). Ожидание...\n", msqid_global);
    signal(SIGINT, broker_sigint_handler);

    struct msgbuf msg;
    while (1) {
        // Читаем только сообщения для брокера (mtype = 1)
        if (msgrcv(msqid_global, &msg, MAX_TEXT, 1, 0) == -1) {
            perror("msgrcv");
            break;
        }

        // Парсим сообщение
        char *command = strtok(msg.mtext, ",");
        if (!command) continue;
        char *pid_str = strtok(NULL, ",");
        if (!pid_str) continue;
        pid_t sender_pid = atoi(pid_str);
        char *topic_payload = strtok(NULL, "");
        if (!topic_payload) continue;

        char *topic = strtok(topic_payload, " ");
        char *payload = strtok(NULL, "");

        if (strcmp(command, "subscribe") == 0) {
            int found = 0;
            for (int i = 0; i < sub_count; i++) {
                if (subscribers[i].pid == sender_pid) {
                    strcpy(subscribers[i].topics[subscribers[i].topic_count++], topic);
                    found = 1;
                    break;
                }
            }
            if (!found && sub_count < MAX_SUBS) {
                subscribers[sub_count].pid = sender_pid;
                strcpy(subscribers[sub_count].topics[0], topic);
                subscribers[sub_count].topic_count = 1;
                sub_count++;
            }
        } 
        else if (strcmp(command, "send") == 0) {
            // Запоминаем издателя
            int pub_found = 0;
            for (int i = 0; i < pub_count; i++) {
                if (pubs_pids[i] == sender_pid) { pub_found = 1; break; }
            }
            if (!pub_found && pub_count < MAX_PUBS) {
                pubs_pids[pub_count++] = sender_pid;
            }

            // Рассылаем подписчикам
            for (int i = 0; i < sub_count; i++) {
                int interested = 0;
                for (int j = 0; j < subscribers[i].topic_count; j++) {
                    if (strcmp(subscribers[i].topics[j], topic) == 0) {
                        interested = 1;
                        break;
                    }
                }
                if (interested) {
                    struct msgbuf out_msg;
                    out_msg.mtype = subscribers[i].pid; // Приоритет = PID подписчика
                    snprintf(out_msg.mtext, MAX_TEXT, "[%s] %s", topic, payload);
                    msgsnd(msqid_global, &out_msg, strlen(out_msg.mtext) + 1, 0);
                }
            }
        }
    }
}
