#pragma once
#include <queue>
#include <pthread.h>

using callback = void (*)(void *arg);

template <typename T>
struct Task
{
    callback function; // c++11
    T *arg;

    Task<T>()
    {
        function = nullptr;
        arg = nullptr;
    }
    Task<T>(callback f, void *arg)
    {
        this->function = f;
        this->arg = (T *)arg;
    }
    
};

template <typename T>
class TaskQueue
{
private:
    pthread_mutex_t m_mtx;
    std::queue<Task<T>> m_taskQ;

public:
    TaskQueue<T>();
    ~TaskQueue<T>();

    void addTask(Task<T> task);
    void addTask(callback f, void *arg);

    Task<T> getTask();

    inline size_t taskNumber()
    {
        return m_taskQ.size();
    }
};