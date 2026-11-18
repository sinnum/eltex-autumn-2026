#include "chat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

mqd_t q_send = (mqd_t)-1;
mqd_t q_recv = (mqd_t)-1;
char q_name_1[256];
char q_name_2[256];
int is_creator = 0;

// обработчик SIGINT / Ctrl+C
void chat_cleanup(int sig) {
    (void)sig;
    char exit_msg[] = "EXIT";
    
    // уведомляем собеседника о выходе с приоритетом 255
    mq_send(q_send, exit_msg, strlen(exit_msg) + 1, PRIO_TERM);
    
    // если мы создавали очередь то удаляем её
    if (is_creator) {
        mq_close(q_send);
        mq_close(q_recv);
        mq_unlink(q_name_1);
        mq_unlink(q_name_2);
        printf("\n[SYSTEM] Queues destroyed. Goodbye.\n");
    } else {
        mq_close(q_send);
        mq_close(q_recv);
    }
    exit(0);
}

// создание очередей или подключение к существующим
void chat_init(const char *base_name) {
    // формируем имена /base_name_1 и /base_name_2
    snprintf(q_name_1, sizeof(q_name_1), "/%s_1", base_name);
    snprintf(q_name_2, sizeof(q_name_2), "/%s_2", base_name);

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;       // максимум 10 сообщений в очереди
    attr.mq_msgsize = MSG_SIZE;
    attr.mq_curmsgs = 0;

    // пытаемся подключиться к уже существующим очередям
    q_send = mq_open(q_name_1, O_WRONLY);
    q_recv = mq_open(q_name_2, O_RDONLY);

    if (q_send == (mqd_t)-1 || q_recv == (mqd_t)-1) {
        // если не получилось значит, мы первые и создаем
        is_creator = 1;
        printf("[SYSTEM] Хост, создаю очереди.\n");
        
        q_recv = mq_open(q_name_1, O_RDONLY | O_CREAT, 0666, &attr);
        q_send = mq_open(q_name_2, O_WRONLY | O_CREAT, 0666, &attr);
        
        if (q_recv == (mqd_t)-1 || q_send == (mqd_t)-1) {
            perror("mq_open");
            exit(1);
        }
    } else {
        printf("[SYSTEM] Гость, подключаюсь к очередям.\n");
    }

    // устанавливаем обработчик Ctrl+C
    signal(SIGINT, chat_cleanup);
}

void chat_run(void) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // дочерний процесс слушает
        char buffer[MSG_SIZE];
        unsigned int prio;
        
        while (1) {
            // Блокирующее чтение из очереди
            ssize_t bytes_read = mq_receive(q_recv, buffer, MSG_SIZE, &prio);
            
            if (bytes_read > 0) {
                // проверяем не прощальное ли это сообщение
                if (prio == PRIO_TERM || strcmp(buffer, "EXIT") == 0) {
                    printf("\n[SYSTEM] Собеседник вышел.\n");
                    break;
                }
                printf("\n[СОБЕСЕДНИК]: %s\n> ", buffer);
                fflush(stdout);
            }
        }
        _exit(0); // завершаем дочерний процесс
    } 
    else {
        // родительский процесс пишет
        char input[MSG_SIZE];
        
        while (1) {
            printf("> ");
            fflush(stdout);
            
            if (fgets(input, sizeof(input), stdin) == NULL) {
                // если нажали Ctrl+D (EOF)
                break;
            }
            
            input[strcspn(input, "\n")] = 0; // удаляем Enter
            
            if (strlen(input) > 0) {
                mq_send(q_send, input, strlen(input) + 1, PRIO_NORMAL);
            }
        }
        
        // если вышли из цикла по EOF вызываем очистку
        chat_cleanup(SIGINT);
    }
}