// Copyright 2018 The genie authors

#ifndef GENIE_THREAD_POOL_H
#define GENIE_THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <thread>
#include <queue>

namespace dsg {

class ThreadPool
{
    // Signal for threads to shut down
    bool running;

    // List of threads
    std::vector <std::thread> threads;

    // Queue of tasks
    std::queue <std::function<void(void)>> tasks;

    // Signal for threads to wake up when tasks are available
    std::condition_variable consumer;

    // Signal for wait() to wake up when queue is empty
    std::condition_variable empty;

    // Access to shared ressources in thread pool
    std::mutex taskAccess;

 public:
    explicit ThreadPool(size_t numThreads);

    // Join all threads after their next task. Wait for all tasks to finish joining.
    void stop();

    // Add a task to the queue
    void pushTask(const std::function<void(void)>& task);

    // Calls stop()
    ~ThreadPool();

    // Block this thread until all tasks in the pool are finished
    void wait();
};
}

#endif //GENIE_THREAD_POOL_H
