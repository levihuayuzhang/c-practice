#include "ThreadPool.h"
#include <cstddef>
#include <cstring>
#include <iostream>
#include <pthread.h>
#include <string>
#include <unistd.h>

template <typename T>
ThreadPool<T>::ThreadPool(int min, int max)
{
    do
    {
        taskQ = new TaskQueue<T>;
        if (taskQ == nullptr)
        {
            std::cout << "malloc taskQ failed..." << std::endl;
            break;
        }

        threadIDs = new pthread_t[max];
        memset(threadIDs, 0, sizeof(pthread_t) * max);

        minNum = min;
        maxNum = max;
        busyNum = 0;
        liveNum = min;
        exitNum = 0;

        if (pthread_mutex_init(&mtxPool, NULL) || pthread_cond_init(&notEmpty, NULL))
        {
            std::cout << "mutex or condition init failed..." << std::endl;
            break;
        }

        shutDown = 0;
        pthread_create(&managerID, NULL, manager, this);
        for (int i = 0; i < min; i++)
        {
            pthread_create(&threadIDs[i], NULL, worker, this);
        }

        return;
    } while (false);

    if (threadIDs)
        delete[] threadIDs;
    if (taskQ)
        delete taskQ;
}

template <typename T>
ThreadPool<T>::~ThreadPool()
{
    shutDown = true;
    pthread_join(managerID, NULL);

    for (int i= 0; i < liveNum; ++i) {
        pthread_cond_signal(&notEmpty);
    }

     delete(taskQ);
     delete(threadIDs);

    pthread_mutex_destroy(&mtxPool);
    pthread_cond_destroy(&notEmpty);
}

template <typename T>
void ThreadPool<T>::addTask(Task<T> task)
{
    if (shutDown)
    {
        return;
    }
    taskQ->addTask(task);
}

template <typename T>
int ThreadPool<T>::getBusyNum()
{
    pthread_mutex_lock(&mtxPool);
    int busyNum = busyNum;
    pthread_mutex_unlock(&mtxPool);
    return busyNum;
}

template <typename T>
int ThreadPool<T>::getAliveNum()
{
    pthread_mutex_lock(&mtxPool);
    int aliveNum = liveNum;
    pthread_mutex_unlock(&mtxPool);
    return aliveNum;
}

template <typename T>
void *ThreadPool<T>::worker(void *arg)
{
    auto *pool = static_cast<ThreadPool<T> *>(arg);
    while (true)
    {
        pthread_mutex_lock(&pool->mtxPool);
        while (!pool->taskQ->taskNumber() && !pool->shutDown) // whether pool is empty
        {
            pthread_cond_wait(&pool->notEmpty, &pool->mtxPool);

            // destroy thread
            if (pool->exitNum > 0)
            {
                pool->exitNum--;
                if (pool->liveNum > pool->minNum)
                {
                    pool->liveNum--;
                    pthread_mutex_unlock(&pool->mtxPool);
                    pool->threadExit();
                }
            }
        }

        if (pool->shutDown)
        {
            pthread_mutex_unlock(&pool->mtxPool);
            pool->threadExit();
        }

        // get task
        Task<T> task = pool->taskQ->getTask();
        pool->busyNum++;
        pthread_mutex_unlock(&pool->mtxPool);

        std::cout << "Thread " << std::to_string((long long)pthread_self()) << " start working..." << std::endl;
        task.function(task.arg); // (*task.function)(task.arg);
        delete task.arg;
        task.arg = nullptr;

        std::cout << "Thread " << std::to_string((long long)pthread_self()) << " end working..." << std::endl;
        pthread_mutex_lock(&pool->mtxPool);
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mtxPool);
    }

    return nullptr;
}

template <typename T>
void *ThreadPool<T>::manager(void *arg)
{
    auto *pool = static_cast<ThreadPool<T> *>(arg);
    while (!pool->shutDown)
    {
        sleep(3);

        pthread_mutex_lock(&pool->mtxPool);
        int queueSize = pool->taskQ->taskNumber();
        int liveNum = pool->liveNum;
        int busyNum = pool->busyNum;
        pthread_mutex_unlock(&pool->mtxPool);

        // add workers (2 for each times)
        if (queueSize > liveNum && liveNum < pool->maxNum)
        {
            pthread_mutex_lock(&pool->mtxPool);
            int counter = 0;
            for (int i = 0; i < pool->maxNum && counter < NUMBER && pool->liveNum < pool->maxNum; ++i)
            {
                if (!pool->threadIDs[i])
                {
                    pthread_create(&pool->threadIDs[i], NULL, worker, pool);
                    counter++;
                    pool->liveNum++;
                }
            }
            pthread_mutex_unlock(&pool->mtxPool);
        }

        // reduce workers (2 for each times)
        if (busyNum * 2 < liveNum && liveNum > pool->minNum)
        {
            pthread_mutex_lock(&pool->mtxPool);
            pool->exitNum = NUMBER;
            pthread_mutex_unlock(&pool->mtxPool);

            for (int i = 0; i < NUMBER; ++i)
            {
                pthread_cond_signal(&pool->notEmpty);
            }
        }
    }
    return nullptr;
}

template <typename T>
void ThreadPool<T>::threadExit()
{
    pthread_t tid = pthread_self();
    for (int i = 0; i < maxNum; ++i)
    {
        if (threadIDs[i] == tid)
        {
            threadIDs[i] = 0;
            std::cout << "threadExit() called: " << std::to_string((long long)pthread_self()) << "exiting..." << std::endl;
            break;
        }
    }
    pthread_exit(nullptr);
}