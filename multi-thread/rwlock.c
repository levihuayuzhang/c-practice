#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define MAX 50

int number;
pthread_rwlock_t rwlock;

void* read_num(void* arg) {
    for (int i = 0; i < MAX; ++i) {
        pthread_rwlock_rdlock(&rwlock);
        printf("Read ===id = %lu, number = %d\n", pthread_self(), number);
        pthread_rwlock_unlock(&rwlock);
        usleep(rand()%5);
    }

    return NULL;
}

void* write_num(void* arg) {
    for (int i = 0; i < MAX; ++i) {
        pthread_rwlock_wrlock(&rwlock);
        int cur = number;
        cur++;
        number = cur;
        printf("                   Write ===id = %lu, number = %d\n", pthread_self(), number);
        pthread_rwlock_unlock(&rwlock);
    }

    return NULL;
}

int main(void) {
    pthread_t p1[5], p2[3];
    pthread_rwlock_init(&rwlock, NULL);

    for (int i = 0; i < 5; ++i) {
        pthread_create(&p1[i], NULL, read_num, NULL);
    }

    for (int i = 0; i < 3; ++i) {
        pthread_create(&p2[i], NULL, write_num, NULL);
    }

    for (int i = 0; i < 3; ++i) {
        pthread_join(p2[i], NULL);
    }

    for (int i = 0; i < 5; ++i) {
        pthread_join(p1[i], NULL);
    }

    pthread_rwlock_destroy(&rwlock);

    return 0;
}