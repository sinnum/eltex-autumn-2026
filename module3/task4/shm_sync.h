#ifndef SHM_SYNC_H
#define SHM_SYNC_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define SHM_SIZE 4096       // размер разделяемой памяти (4 КБ)
#define MAX_NUMS 50         // максимум чисел в одном блоке
#define NUM_BLOCKS 10       // сколько блоков помещается в память

// ключи IPC
#define SHM_KEY_PATH "/tmp/shm_prod_cons"
#define SHM_KEY_ID 'S'

#define SEM_MUTEX 0
#define SEM_EMPTY 1
#define SEM_FULL  2

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// структура блока в разделяемой памяти
typedef struct {
    int count;          // количество элементов (0 = обработан)
    int next_offset;    // смещение следующего блока (0 = конец)
    int data[MAX_NUMS]; 
} block_t;

// структура-заголовок 
typedef struct {
    int head_offset;    // смещение первого блока (-1 если пусто)
    int tail_offset;    // смещение последнего блока (-1 если пусто)
    int free_offset;    // смещение для записи следующего блока
} shm_header_t;

int init_resources();
void destroy_resources(int shmid, int semid);
void P(int semid, int semnum);
void V(int semid, int semnum);
int get_shm_segment(int *shmid, int *semid);

#endif