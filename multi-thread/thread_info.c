#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

struct Man{
    int num;
    int age;
}Man;

void* callback(void* arg){
    for (int i = 0; i < 5; ++i) {
        printf("callback printing loop %d\n", i);
        printf("callback thread id is %ld\n", pthread_self());
    }
    struct Man* man = (struct Man*) arg;
    man->age = 30;
    man->num = 1;
    printf("print from child thread %d, %d\n", man->age, man->num); 
    pthread_exit(&man);

    return NULL;
}

int main(void) {
    struct Man man;
    
    pthread_t tid = pthread_self();
    printf("Current thread ID is %ld\n", tid);
    pthread_create(&tid, NULL, callback, &man);
    printf("Created thread ID %d\n", tid);

    void* ptr;
    pthread_join(tid, &ptr); // sync
    // struct Man* man = (struct Man*)ptr;
    printf("print from main thread %d, %d\n", man.age, man.num);

    // pthread_detach(tid);
    // pthread_exit(NULL);

    return 0;
}