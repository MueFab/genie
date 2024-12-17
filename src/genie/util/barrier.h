/**
 * Copyright 2018-2024 The Genie Authors.
 * @file barrier.h
 * @brief Barrier synchronization primitive for multiple threads.
 *
 * This file contains the definition of the Barrier class, which is used to
 * synchronize multiple threads.
 *
 * @details The Barrier class provides a mechanism for multiple threads to wait
 * until all threads have reached a certain point in their execution. It uses a
 * mutex and a condition variable to achieve this synchronization.
 *
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_BARRIER_H_
#define SRC_GENIE_UTIL_BARRIER_H_

// -----------------------------------------------------------------------------

#include <condition_variable>
#include <mutex>

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Barrier synchronization primitive for multiple threads.
 */
class Barrier {
 public:
  /**
   * @brief Construct a new Barrier object.
   *
   * @param num_threads Number of threads to synchronize.
   */
  explicit Barrier(int num_threads);

  /**
   * @brief Wait for all threads to reach the barrier.
   */
  void wait();

 private:
  /// Mutex for synchronization.
  std::mutex mutex_;

  /// Condition variable for synchronization.
  std::condition_variable cond_;

  /// Number of threads to synchronize.
  int num_threads_;

  /// Number of threads that have reached the barrier.
  int count_;

  /// Barrier generation counter.
  int generation_;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_BARRIER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
