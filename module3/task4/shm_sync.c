#include "shm_sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void P(int semid, int semnum) {
    struct sembuf sb = {semnum, -1, 0};
    if (semop(semid, &sb, 1) == -1) {
        perror("semop P");
        exit(1);
    }
}

void V(int semid, int semnum) {
    struct sembuf sb = {semnum, 1, 0};
    if (semop(semid, &sb, 1) == -1) {
        perror("semop V");
        exit(1);
    }
}

int init_resources() {
    key_t key = ftok(SHM_KEY_PATH, SHM_KEY_ID);
    if (key == -1) { perror("ftok"); exit(1); }

    // создаем разделяемую память
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) { perror("shmget"); exit(1); }

    // создаем набор из 3 семафоров
    int semid = semget(key, 3, IPC_CREAT | 0666);
    if (semid == -1) { perror("semget"); exit(1); }

    return shmid; // для простоты возвращаем shmid
}

int get_shm_segment(int *shmid, int *semid) {
    key_t key = ftok(SHM_KEY_PATH, SHM_KEY_ID);
    *shmid = shmget(key, SHM_SIZE, 0666);
    *semid = semget(key, 3, 0666);
    if (*shmid == -1 || *semid == -1) return -1;
    return 0;
}

void destroy_resources(int shmid, int semid) {
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
}
