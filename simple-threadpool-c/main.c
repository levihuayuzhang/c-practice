#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "threadpool.h"

void taskFunc(void *arg)
{
    int num = *(int *)arg;
    printf("Thread %ld is working, number = %d\n", pthread_self(), num);
    sleep(1);
}

int main(int argc, char const *argv[])
{
    ThreadPool<T> *pool= ThreadPoolCreate(3, 10, 100);
    for (int i = 0; i < 100; ++i)
    {
        int *num = (int *)malloc(sizeof(int));
        *num = i + 100;
        threadPoolAdd(pool, taskFunc, num);
    }

    sleep(30);

    threadPoolDestroy(pool);

    return 0;
}
