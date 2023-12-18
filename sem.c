#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

sem_t semp, semc;

struct Node {
    int number;
    struct Node *next;
}Node;
struct Node *head = NULL;


void * producer (void *arg) {
    while(1) {
        sem_wait(&semp);
        struct Node * newNode = (struct Node *)malloc(sizeof(struct Node));
        newNode->number = rand()%1000;
        newNode->next = head;
        head = newNode;
        printf("producer ID %ld, number is %d\n", pthread_self(), newNode->number);
        sem_post(&semc);
        sleep(rand()%3);
    }

    return NULL;
}

void * consumer (void *arg) {
    while(1) {
        sem_wait(&semc);
        struct Node * node = head;
        printf("             consumer ID %ld, number is %d\n", pthread_self(), node->number);
        head = head->next;
        free(node);
        sem_post(&semp);
        sleep(rand()%3);
    }

    return NULL;
}


int main(int argc, char **argv) {
    sem_init(&semp, 0, 1);
    sem_init(&semc, 0, 0);

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

    sem_destroy(&semp);
    sem_destroy(&semc);

    return 0;
}