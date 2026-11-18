#include "shm_sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern void P(int semid, int semnum);
extern void V(int semid, int semnum);
extern int get_shm_segment(int *shmid, int *semid);

void run_consumer() {
    int shmid, semid;
    if (get_shm_segment(&shmid, &semid) == -1) {
        printf("[CONSUMER] No resources found. Is producer running?\n");
        return;
    }

    shm_header_t *header = (shm_header_t *)shmat(shmid, NULL, 0);
    printf("[CONSUMER] Connected to shared memory.\n");

    while (1) {
        // ждем появления заполненного блока
        P(semid, SEM_FULL);

        P(semid, SEM_MUTEX);

        // ищем первый необработанный блок
        int offset = header->head_offset;
        block_t *target = NULL;

        while (offset != -1) {
            block_t *b = (block_t *)((char *)header + offset);
            if (b->count > 0) {
                target = b;
                break;
            }
            offset = b->next_offset;
        }

        if (target != NULL) {
            // ищем минимум и максимум
            int min = target->data[0];
            int max = target->data[0];
            for (int i = 1; i < target->count; i++) {
                if (target->data[i] < min) min = target->data[i];
                if (target->data[i] > max) max = target->data[i];
            }
            printf("[CONSUMER] Processed block: count=%d, min=%d, max=%d\n", 
                   target->count, min, max);
            
            // помечаем как обработанный
            target->count = 0;
        }

        V(semid, SEM_MUTEX);
        // увеличиваем счетчик свободных блоков
        V(semid, SEM_EMPTY); 

        sleep(1); 
    }

    shmdt(header);
}
