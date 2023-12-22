#ifndef _THREADPOOL_H
#define _THREADPOOL_H

typedef struct ThreadPool ThreadPool;

// create and initialize thread pool
ThreadPool * ThreadPoolCreate(int min, int max, int queueSize);

// add task to thread pool
void threadPoolAdd(ThreadPool* pool, void(*func)(void *), void *arg);

// destroy thread pool
int threadPoolDestroy(ThreadPool* pool);

// get working thread number
int threadPoolBusyNum(ThreadPool* pool);

// get number of threads that still alive but not working
int threadPoolAliveNum(ThreadPool* pool);

void *worker(void *arg);

void *manager(void *arg);

void *threadExit(ThreadPool* pool);

#endif