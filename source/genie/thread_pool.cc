// Copyright 2018 The genie authors

#include "genie/thread_pool.h"

#include <iostream>

namespace dsg
{
    ThreadPool::ThreadPool(size_t numThreads) : running(true){
        for (size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back(
                    [this, i]
                    {
                        std::unique_lock <std::mutex> lk(this->taskAccess);
                        std::cout << "Launched thread " << i << "!" << std::endl;
                        while (this->running) {
                            // Tasks available
                            if (!this->tasks.empty()) {

                                // Get new task and run it
                                std::function<void(void)> task = this->tasks.front();
                                this->tasks.pop();

                                // Don't block other threads while running
                                lk.unlock();

                                task();

                                lk.lock();
                            }

                            // Ran out of tasks
                            if (this->tasks.empty()) {
                                empty.notify_all();
                                if (this->running) {
                                    // Block and wait for new tasks
                                    consumer.wait(lk);
                                }
                            }
                        }
                        std::cout << "Left thread " << i << "!" << std::endl;
                    }
            );
        }
    }

    void ThreadPool::stop(){
        std::unique_lock <std::mutex> lk(this->taskAccess);
        running = false;
        lk.unlock();
        consumer.notify_all();
        for (auto& t : threads) {
            t.join();
        }
    }

    void ThreadPool::pushTask(const std::function<void(void)>& task){
        std::unique_lock <std::mutex> lock(taskAccess);
        tasks.push(task);
        lock.unlock();
        consumer.notify_one();
    }

    ThreadPool::~ThreadPool(){
        stop();
    }

    void ThreadPool::wait(){
        std::unique_lock <std::mutex> lock(taskAccess);
        if (!tasks.empty()) {
            empty.wait(lock);
        }
    }
}