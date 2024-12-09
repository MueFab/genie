/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_DYNAMIC_SCHEDULER_H_
#define SRC_GENIE_READ_SPRING_DYNAMIC_SCHEDULER_H_

#include <algorithm>
#include <atomic>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

class DynamicScheduler {
 public:
  explicit DynamicScheduler(const int num_threads)
      : num_threads_(num_threads), current_task_(0) {
    if (num_threads_ <= 0) {
      num_threads_ = std::max(1u, std::thread::hardware_concurrency());
    }
  }

  // Run tasks with dynamic scheduling
  void run(const size_t total_tasks,
           const std::function<void(size_t)>& task_function) {
    current_task_.store(0);  // Reset task counter
    threads_.clear();        // Clear any previously used threads

    // Launch threads
    for (int i = 0; i < num_threads_; ++i) {
      threads_.emplace_back([&, i]() {
        while (true) {
          // Fetch the next task ID atomically
          const size_t task_id = current_task_.fetch_add(1);

          // Exit if all tasks are processed
          if (task_id >= total_tasks) {
            break;
          }

          // Execute the task
          task_function(task_id);
        }
      });
    }

    // Join threads
    for (auto& thread : threads_) {
      thread.join();
    }
  }

 private:
  int num_threads_;                   // Number of threads
  std::atomic<size_t> current_task_;  // Atomic counter for tasks
  std::vector<std::thread> threads_;  // Thread pool
};

#endif  // SRC_GENIE_READ_SPRING_DYNAMIC_SCHEDULER_H_
