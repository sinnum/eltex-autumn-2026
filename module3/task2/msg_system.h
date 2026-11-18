#ifndef MSG_SYSTEM_H
#define MSG_SYSTEM_H

#include <sys/types.h>

#define MSG_KEY 0x1234
#define MAX_TEXT 256

struct msgbuf {
    long mtype;
    char mtext[MAX_TEXT];
};

void run_broker(void);
void run_publisher(const char *topic);
void run_subscriber(int topic_count, char *topics[]);

#endif // MSG_SYSTEM_H
