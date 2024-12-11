/**
 * Copyright 2018-2024 The Genie Authors.
 * @file barrier.h
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
  std::mutex mutex_;  //!< @brief Mutex for synchronization.
  /// @brief Condition variable for synchronization.
  std::condition_variable cond_;
  int num_threads_;  //!< @brief Number of threads to synchronize.
  int count_;       //!< @brief Number of threads that have reached the barrier.
  int generation_;  //!< @brief Barrier generation counter.
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_BARRIER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
