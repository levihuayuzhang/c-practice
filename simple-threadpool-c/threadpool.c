#include "threadpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

// const int NUMBER = 2;
#define NUMBER 2

typedef struct Task<T>
{
    void (*function)(void *arg);
    void *arg;
} Task<T>;

struct ThreadPool<T>
{
    Task<T> *taskQ;
    int queueCapacity;
    int queueSize;
    int queueFront;
    int queueRear;

    pthread_t manager_ID;
    pthread_t *thread_IDs;

    int minNum;
    int maxNum;
    int busyNum; // current running works
    int liveNum; // not running and not destroyed
    int exitNum; // exited

    pthread_mutex_t mtxThreadPool;
    pthread_mutex_t mtxBusy; // more flexibility for fequently changed variable
    pthread_cond_t not_full;
    pthread_cond_t not_empty;

    int shutDown; // 1 for yes (shutdown)
};

ThreadPool<T> *ThreadPoolCreate(int min, int max, int queueSize)
{
    ThreadPool<T> *pool = (ThreadPool<T> *)malloc(sizeof(ThreadPool<T>));
    do
    {

        if (pool == NULL)
        {
            printf("Malloc threadpool failed...\n");
            break;
        }

        pool->thread_IDs = malloc(sizeof(pthread_t) * max);
        if (pool->thread_IDs == NULL)
        {
            printf("Malloc thread_IDs failed...\n");
            break;
        }
        memset(pool->thread_IDs, 0, sizeof(pthread_t) * max);

        pool->minNum = min;
        pool->maxNum = max;
        pool->busyNum = 0;
        pool->liveNum = min;
        pool->exitNum = 0;
        pool->shutDown = 0;

        if (pthread_mutex_init(&pool->mtxThreadPool, NULL) ||
            pthread_mutex_init(&pool->mtxBusy, NULL) ||
            pthread_cond_init(&pool->not_full, NULL) ||
            pthread_cond_init(&pool->not_empty, NULL))
        {
            printf("Malloc thread locks failed...\n");
            break;
        }

        pool->queueCapacity = queueSize;
        pool->taskQ = (Task<T> *)malloc(sizeof(Task<T>) * queueSize);
        pool->queueSize = 0; // current task number
        pool->queueFront = 0;
        pool->queueRear = 0;

        pthread_create(&pool->manager_ID, NULL, manager, pool);
        for (int i = 0; i < min; ++i)
        {
            pthread_create(&pool->thread_IDs[i], NULL, worker, pool);
        }

        return pool;
    } while (0);

    // release resources
    if (pool && pool->thread_IDs)
        free(pool->thread_IDs);
    if (pool && pool->taskQ)
        free(pool->taskQ);
    if (pool)
        free(pool);

    return NULL;
}

void threadPoolAdd(ThreadPool<T> *pool, void (*func)(void *arg), void *arg)
{
    pthread_mutex_lock(&pool->mtxThreadPool);
    while(pool->queueSize == pool->queueCapacity && !pool->shutDown)
    {
        pthread_cond_wait(&pool->not_full, &pool->mtxThreadPool);
    }
    if (pool->shutDown)
    {
        pthread_mutex_unlock(&pool->mtxThreadPool);
        return;
    }
    pool->taskQ[pool->queueRear].function = func;
    pool->taskQ[pool->queueRear].arg = arg;
    pool->queueRear =  (pool->queueRear + 1) % pool->queueCapacity;
    pool->queueSize++;

    pthread_cond_signal(&pool->not_empty);

    pthread_mutex_unlock(&pool->mtxThreadPool);

}

int threadPoolDestroy(ThreadPool<T> *pool)
{
    if (pool == NULL) return -1;
    pool->shutDown = 1;
    pthread_join(pool->manager_ID, NULL);

    for (int i= 0; i < pool->liveNum; ++i) {
        pthread_cond_signal(&pool->not_empty);
    }

    if (pool->taskQ) free(pool->taskQ);
    if (pool->thread_IDs) free(pool->thread_IDs);

    pthread_mutex_destroy(&pool->mtxThreadPool);
    pthread_mutex_destroy(&pool->mtxBusy);
    pthread_cond_destroy(&pool->not_empty);
    pthread_cond_destroy(&pool->not_full);

    free(pool);
    pool = NULL;

    return 0;
}

int threadPoolBusyNum(ThreadPool<T> *pool)
{
    pthread_mutex_lock(&pool->mtxBusy);
    int busyNum = pool->busyNum;
    pthread_mutex_unlock(&pool->mtxBusy);
    return busyNum;
}

int threadPoolAliveNum(ThreadPool<T> *pool)
{
    pthread_mutex_lock(&pool->mtxThreadPool);
    int aliveNum = pool->liveNum;
    pthread_mutex_unlock(&pool->mtxThreadPool);
    return aliveNum;
}

void *worker(void *arg)
{
    ThreadPool<T> *pool = (ThreadPool<T> *)arg;
    while (1)
    {
        pthread_mutex_lock(&pool->mtxThreadPool);
        while (!pool->queueSize && !pool->shutDown)
        {
            pthread_cond_wait(&pool->not_empty, &pool->mtxThreadPool);

            // destroy thread
            if (pool->exitNum > 0)
            {
                pool->exitNum--;
                if (pool->liveNum > pool->minNum)
                {
                    pool->liveNum--;
                    pthread_mutex_unlock(&pool->mtxThreadPool);
                    threadExit(pool);
                }
            }
        }

        if (pool->shutDown)
        {
            pthread_mutex_unlock(&pool->mtxThreadPool);
            threadExit(pool);
        }

        // get task
        Task<T> task;
        task.function = pool->taskQ[pool->queueFront].function;
        task.arg = pool->taskQ[pool->queueFront].arg;

        pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity; // circular queue
        pool->queueSize--;
        pthread_cond_signal(&pool->not_full);
        pthread_mutex_unlock(&pool->mtxThreadPool);

        printf("thread %ld start working...\n", pthread_self());
        pthread_mutex_lock(&pool->mtxBusy);
        pool->busyNum++;
        pthread_mutex_unlock(&pool->mtxBusy);
        task.function(task.arg); // (*task.function)(task.arg);
        free(task.arg);
        task.arg = NULL;

        printf("thread %ld end working...\n", pthread_self());
        pthread_mutex_lock(&pool->mtxBusy);
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mtxBusy);
    }

    return NULL;
}

void *manager(void *arg)
{
    ThreadPool<T> *pool = (ThreadPool<T> *)arg;
    while (!pool->shutDown)
    {
        sleep(3);

        pthread_mutex_lock(&pool->mtxThreadPool);
        int queueSize = pool->queueSize;
        int liveNum = pool->liveNum;
        pthread_mutex_unlock(&pool->mtxThreadPool);

        pthread_mutex_lock(&pool->mtxBusy);
        int busyNum = pool->busyNum;
        pthread_mutex_unlock(&pool->mtxBusy);

        // add workers (2 for each times)
        if (queueSize > liveNum && liveNum < pool->maxNum)
        {
            pthread_mutex_lock(&pool->mtxThreadPool);
            int counter = 0;
            for (int i = 0; i < pool->maxNum && counter < NUMBER && pool->liveNum < pool->maxNum; ++i)
            {
                if (!pool->thread_IDs[i])
                {
                    pthread_create(&pool->thread_IDs[i], NULL, worker, pool);
                    counter++;
                    pool->liveNum++;
                }
            }
            pthread_mutex_unlock(&pool->mtxThreadPool);
        }

        // reduce workers (2 for each times)
        if (busyNum * 2 < liveNum && liveNum > pool->minNum)
        {
            pthread_mutex_lock(&pool->mtxThreadPool);
            pool->exitNum = NUMBER;
            pthread_mutex_unlock(&pool->mtxThreadPool);

            for (int i = 0; i < NUMBER; ++i)
            {
                pthread_cond_signal(&pool->not_empty);
            }
        }
    }
    return NULL;
}

void *threadExit(ThreadPool<T> *pool)
{
    pthread_t tid = pthread_self();
    for (int i = 0; i < pool->maxNum; ++i)
    {
        if (pool->thread_IDs[i] == tid)
        {
            pool->thread_IDs[i] = 0;
            printf("threadExit() called: %ld exiting...\n", tid);
            break;
        }
    }
    pthread_exit(NULL);
}