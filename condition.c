#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_cond_t cond;
pthread_mutex_t mtx;

struct Node {
    int number;
    struct Node *next;
}Node;
struct Node *head = NULL;


void * producer (void *arg) {
    while(1) {
        pthread_mutex_lock(&mtx);
        struct Node * newNode = (struct Node *)malloc(sizeof(struct Node));
        newNode->number = rand()%1000;
        newNode->next = head;
        head = newNode;
        printf("producer ID %ld, number is %d\n", pthread_self(), newNode->number);
        pthread_mutex_unlock(&mtx);
        pthread_cond_broadcast(&cond);
        sleep(rand()%3);
    }

    return NULL;
}

void * consumer (void *arg) {
    while(1) {
        pthread_mutex_lock(&mtx);
        while(head==NULL) {
            pthread_cond_wait(&cond, &mtx);
        }
        struct Node * node = head;
        printf("             consumer ID %ld, number is %d\n", pthread_self(), node->number);
        head = head->next;
        free(node);
        pthread_mutex_unlock(&mtx);
        sleep(rand()%3);
    }

    return NULL;
}


int main(int argc, char **argv) {
    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_t t1[5], t2[5];
    for (int i = 0; i < 5; ++i) {
        pthread_create(&t1[i], NULL, producer, NULL);
    }
    for (int i = 0; i < 5; ++i) {
        pthread_create(&t2[i], NULL, consumer, NULL);
    }



    for (int i = 0; i < 5; ++i) {
        pthread_join(t1[i], NULL);
        pthread_join(t2[i], NULL);
    }
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cond);

    return 0;
}