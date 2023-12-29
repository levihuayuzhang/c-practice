#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <pthread.h>
#include <iostream>
#include "ThreadPool.h"
#include "ThreadPool.cpp"

void taskFunc(void *arg)
{
    int num = *(int *)arg;
    std::cout << "Thread " << std::to_string((long long)pthread_self()) << " is working, number = " << num << std::endl;
    sleep(1);
}

int main(int argc, char const *argv[])
{
    ThreadPool<int> pool(3, 10);
    for (int i = 0; i < 100; ++i)
    {
        int *num = new int(i+100);
        pool.addTask(Task<int>(taskFunc, num));
    }

    sleep(20);

    return 0;
}
