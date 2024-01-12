#include "ThreadPool.cpp"
#include "ThreadPool.h"
#include <iostream>
#include <pthread.h>
#include <unistd.h>

void taskFunc(void *arg) {
  int num = *static_cast<int *>(arg);
  std::cout << "Thread " << std::to_string((long long)pthread_self())
            << " is working, number = " << num << "\n";
  sleep(1);
}

int main(int argc, char const *argv[]) {
  ThreadPool<int> pool(3, 10);
  for (int i = 0; i < 100; ++i) {
    int *num = new int(i + 100);
    pool.addTask(Task<int>(taskFunc, num));
  }

  sleep(20);

  return 0;
}
