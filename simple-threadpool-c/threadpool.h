#ifndef _THREADPOOL_H
#define _THREADPOOL_H

typedef struct ThreadPool<T> ThreadPool<T>;

// create and initialize thread pool
ThreadPool<T> * ThreadPoolCreate(int min, int max, int queueSize);

// add task to thread pool
void threadPoolAdd(ThreadPool<T>* pool, void(*func)(void *), void *arg);

// destroy thread pool
int threadPoolDestroy(ThreadPool<T>* pool);

// get working thread number
int threadPoolBusyNum(ThreadPool<T>* pool);

// get number of threads that still alive but not working
int threadPoolAliveNum(ThreadPool<T>* pool);

void *worker(void *arg);

void *manager(void *arg);

void *threadExit(ThreadPool<T>* pool);

#endif