/**
 * Copyright 2018-2024 The Genie Authors.
 * @file barrier.cc
 * @brief Implementation of the Barrier synchronization primitive for multiple
 * threads.
 *
 * This file contains the implementation of the Barrier class, which is used to
 * synchronize multiple threads.
 *
 * @details The Barrier class provides a mechanism for multiple threads to wait
 * until all threads have reached a certain point in their execution. It uses a
 * mutex and a condition variable to achieve this synchronization.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

// -----------------------------------------------------------------------------

#include "genie/util/barrier.h"

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

Barrier::Barrier(const int num_threads)
    : num_threads_(num_threads), count_(0), generation_(0) {}

// -----------------------------------------------------------------------------

void Barrier::wait() {
  std::unique_lock lock(mutex_);
  int current_generation = generation_;

  if (++count_ == num_threads_) {
    // Last thread to arrive at the barrier
    count_ = 0;
    generation_++;
    cond_.notify_all();
  } else {
    // Wait for all threads to reach the barrier
    cond_.wait(lock, [this, current_generation] {
      return generation_ != current_generation;
    });
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
