/**
 * Copyright 2018-2024 The Genie Authors.
 * @file dynamic_scheduler.h
 *
 * @brief Implementation of the DynamicScheduler class for managing dynamic task
 * scheduling.
 *
 * This file contains the implementation of the DynamicScheduler class, which
 * provides functionality for dynamically scheduling and executing tasks across
 * multiple threads. It includes methods for initializing the scheduler, running
 * tasks, and managing thread execution.
 *
 * *details The DynamicScheduler class is designed to efficiently distribute
 * tasks among available threads, ensuring optimal utilization of system
 * resources. It supports dynamic task assignment, allowing tasks to be fetched
 * and executed by threads as they become available.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_DYNAMIC_SCHEDULER_H_
#define SRC_GENIE_UTIL_DYNAMIC_SCHEDULER_H_

// -----------------------------------------------------------------------------

#include <atomic>
#include <functional>
#include <thread>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Dynamic scheduler for running tasks with dynamic scheduling.
 */
class DynamicScheduler {
 public:
  /**
   * @brief Information about the current task being executed by a thread.
   */
  struct SchedulerInfo {
    /// ID of the thread executing the task
    size_t thread_id;

    /// ID of the task being executed
    size_t task_id;
  };

  /**
   * @brief Construct a new DynamicScheduler object.
   *
   * @param num_threads Number of threads to use for running tasks.
   */
  explicit DynamicScheduler(size_t num_threads);

  /**
   * @brief Run tasks with dynamic scheduling.
   *
   * This function runs a set of tasks with dynamic scheduling, where each task
   * is executed by a thread as soon as it becomes available. The tasks are
   * executed in parallel by multiple threads, with each thread executing one
   * task at a time.
   *
   * @param total_tasks Total number of tasks to run.
   * @param task_function Function to execute for each task.
   */
  void run(size_t total_tasks,
           const std::function<void(const SchedulerInfo&)>& task_function);

 private:
  /// Number of threads to use
  size_t num_threads_;

  /// Current task being executed
  std::atomic<size_t> current_task_;

  /// Threads
  std::vector<std::thread> threads_;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_DYNAMIC_SCHEDULER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
