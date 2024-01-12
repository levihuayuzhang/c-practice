#include "TaskQueue.h"

template <typename T>
TaskQueue<T>::TaskQueue()
{
    pthread_mutex_init(&m_mtx, nullptr);
}

template <typename T>
TaskQueue<T>::~TaskQueue()
{
    pthread_mutex_destroy(&m_mtx);
}

template <typename T>
void TaskQueue<T>::addTask(Task<T> task)
{
    pthread_mutex_lock(&m_mtx);
    m_taskQ.push(task);
    pthread_mutex_unlock(&m_mtx);
}

template <typename T>
void TaskQueue<T>::addTask(callback f, void *arg)
{
    pthread_mutex_lock(&m_mtx);
    m_taskQ.push(Task<T>(f, arg));
    pthread_mutex_unlock(&m_mtx);
}

template <typename T>
Task<T> TaskQueue<T>::getTask()
{
    pthread_mutex_lock(&m_mtx);
    Task<T> t;
    if (!m_taskQ.empty())
    {
        t = m_taskQ.front();
        m_taskQ.pop();
    }
    pthread_mutex_unlock(&m_mtx);

    return t;
}
