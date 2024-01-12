#pragma once
#include "TaskQueue.h"
#include "TaskQueue.cpp"

template <typename T>
class ThreadPool
{
public:
    ThreadPool<T>(int min, int max);
    ~ThreadPool<T>();

    void addTask(Task<T> task);
    int getBusyNum();
    int getAliveNum();

private:
    static void* worker(void* arg);
    static void* manager(void* arg);
    void threadExit();

    TaskQueue<T>* taskQ;
    static const int NUMBER = 2;

    pthread_t managerID;
    pthread_t* threadIDs;

    int minNum;
    int maxNum;
    int busyNum;
    int liveNum;
    int exitNum; // prepared to exiting

    pthread_mutex_t mtxPool;
    pthread_cond_t notEmpty;

    bool shutDown;
};