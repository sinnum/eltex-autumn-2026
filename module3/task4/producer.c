#include "shm_sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

extern void P(int semid, int semnum);
extern void V(int semid, int semnum);

void run_producer() {
    int shmid, semid;
    int is_creator = 0;

    // если не получаем доуступ то мы первые и создаем.
    if (get_shm_segment(&shmid, &semid) == -1) {
        is_creator = 1;
        shmid = init_resources();
        semid = semget(ftok(SHM_KEY_PATH, SHM_KEY_ID), 3, 0666);
        
        // инициализация семафоров
        union semun arg;
        arg.val = 1; semctl(semid, SEM_MUTEX, SETVAL, arg); // мьютекс = 1
        arg.val = NUM_BLOCKS; semctl(semid, SEM_EMPTY, SETVAL, arg); // свободных блоков
        arg.val = 0; semctl(semid, SEM_FULL, SETVAL, arg); // заполненных блоков

        // инициализация заголовка памяти
        shm_header_t *header = (shm_header_t *)shmat(shmid, NULL, 0);
        header->head_offset = -1;
        header->tail_offset = -1;
        header->free_offset = sizeof(shm_header_t);
        shmdt(header);
        printf("[PRODUCER] Resources initialized.\n");
    } else {
        printf("[PRODUCER] Connected to existing resources.\n");
    }

    shm_header_t *header = (shm_header_t *)shmat(shmid, NULL, 0);
    srand(time(NULL) ^ getpid()); // уникальный seed для каждого процесса

    while (1) {
        // ждем свободного блока (P на SEM_EMPTY)
        // если память закончится producer заблокируется
        P(semid, SEM_EMPTY);

        // захватываем мьютекс
        P(semid, SEM_MUTEX);

        // проверяем, не пора ли заканчивать
        if (header->free_offset + sizeof(block_t) > SHM_SIZE) {
            V(semid, SEM_MUTEX);
            printf("[PRODUCER] Memory is full. Waiting for consumers...\n");
            // возвращаем семафор обратно так как мы не использовали слот
            V(semid, SEM_EMPTY); 
            break; // выходим из цикла генерации
        }

        block_t *new_block = (block_t *)((char *)header + header->free_offset);
        new_block->count = rand() % MAX_NUMS + 1; // тт 1 до 50 чисел
        new_block->next_offset = -1; // пока не знаем следующий

        // заполняем случайными числами
        for (int i = 0; i < new_block->count; i++) {
            new_block->data[i] = rand() % 1000;
        }

        // привязываем к списку
        if (header->head_offset == -1) {
            header->head_offset = header->free_offset;
        } else {
            block_t *prev = (block_t *)((char *)header + header->tail_offset);
            prev->next_offset = header->free_offset;
        }
        header->tail_offset = header->free_offset;
        header->free_offset += sizeof(block_t);

        V(semid, SEM_MUTEX);
        // сообщаем потребителям, что появился новый блок (V на SEM_FULL)
        V(semid, SEM_FULL);

        printf("[PRODUCER] Generated block with %d items.\n", new_block->count);
        sleep(rand() % 2 + 1); // Задержка генерации
    }

    // после выхода ждем пока потребители обработают все блоки
    printf("[PRODUCER] Generation done. Waiting for all blocks to be processed...\n");
    while (1) {
        P(semid, SEM_MUTEX);
        int all_processed = 1;
        int offset = header->head_offset;
        while (offset != -1) {
            block_t *b = (block_t *)((char *)header + offset);
            if (b->count > 0) {
                all_processed = 0;
                break;
            }
            offset = b->next_offset;
        }
        V(semid, SEM_MUTEX);

        if (all_processed) {
            printf("[PRODUCER] All blocks processed. Cleaning up.\n");
            break;
        }
        sleep(2);
    }

    shmdt(header);
    if (is_creator) {
        destroy_resources(shmid, semid);
        printf("[PRODUCER] Resources destroyed.\n");
    }
}
