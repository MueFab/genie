/**
 * Copyright 2018-2024 The Genie Authors.
 * @file dynamic_scheduler.cc
 *
 * @brief Implementation of the DynamicScheduler class for managing dynamic task
 * scheduling.
 *
 * This file contains the implementation of the DynamicScheduler class, which
 * provides functionality for dynamically scheduling and executing tasks across
 * multiple threads. It includes methods for initializing the scheduler, running
 * tasks, and managing thread execution.
 *
 * @details The DynamicScheduler class is designed to efficiently distribute
 * tasks among available threads, ensuring optimal utilization of system
 * resources. It supports dynamic task assignment, allowing tasks to be fetched
 * and executed by threads as they become available.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

// -----------------------------------------------------------------------------

#include "genie/util/dynamic_scheduler.h"

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

DynamicScheduler::DynamicScheduler(const size_t num_threads)
    : num_threads_(num_threads), current_task_(0) {
  if (num_threads_ <= 0) {
    num_threads_ = std::max(1u, std::thread::hardware_concurrency());
  }
}

// -----------------------------------------------------------------------------

void DynamicScheduler::run(
    const size_t total_tasks,
    const std::function<void(const SchedulerInfo&)>& task_function) {
  current_task_.store(0);  // Reset task counter
  threads_.clear();        // Clear any previously used threads

  // Launch threads
  for (size_t i = 0; i < num_threads_; ++i) {
    threads_.emplace_back([&, i] {
      while (true) {
        // Fetch the next task ID atomically
        const size_t task_id = current_task_.fetch_add(1);

        // Exit if all tasks are processed
        if (task_id >= total_tasks) {
          break;
        }

        const SchedulerInfo info = {i, task_id};
        // Execute the task
        task_function(info);
      }
    });
  }

  // Join threads
  for (auto& thread : threads_) {
    thread.join();
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
