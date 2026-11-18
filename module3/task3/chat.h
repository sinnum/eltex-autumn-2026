#ifndef CHAT_H
#define CHAT_H

#include <mqueue.h> 

#define MSG_SIZE 256
#define PRIO_NORMAL 1
#define PRIO_TERM 255

extern mqd_t q_send, q_recv;
extern char q_name_1[256], q_name_2[256];
extern int is_creator;

void chat_init(const char *base_name);
void chat_run(void);
void chat_cleanup(int sig);

#endif // CHAT_H