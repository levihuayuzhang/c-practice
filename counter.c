#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define MAX 50

int number;
pthread_mutex_t mtx;

void* funcA_num(void* arg) {
    for (int i = 0; i < MAX; ++i) {
        pthread_mutex_lock(&mtx);
        int cur = number;
        cur++;
        number = cur;
        printf("A ===id = %lu, number = %d\n", pthread_self(), number);
        pthread_mutex_unlock(&mtx);
    }

    return NULL;
}

void* funcB_num(void* arg) {
    for (int i = 0; i < MAX; ++i) {
        pthread_mutex_lock(&mtx);
        int cur = number;
        cur++;
        number = cur;
        printf("                   B ===id = %lu, number = %d\n", pthread_self(), number);
        pthread_mutex_unlock(&mtx);
    }

    return NULL;
}

int main(void) {
    pthread_t p1, p2;
    pthread_mutex_init(&mtx, NULL);

    pthread_create(&p1, NULL, funcA_num, NULL);
    pthread_create(&p2, NULL, funcB_num, NULL);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    pthread_mutex_destroy(&mtx);

    return 0;
}