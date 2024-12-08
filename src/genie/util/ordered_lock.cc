/**
 * Copyright 2018-2024 The Genie Authors.
 * @file ordered_lock.cc
 * @brief Implementation of the OrderedLock class for enforcing execution order
 * in multithreaded environments.
 *
 * This file contains the implementation of the OrderedLock class, which is
 * designed to synchronize multiple threads and ensure that they execute in a
 * specified order. It uses a counter-based approach to track the order of
 * execution and condition variables to block or release threads based on their
 * assigned order.
 *
 * @details The OrderedLock class provides methods for waiting until a thread's
 * turn is reached, marking a thread as finished, and resetting the internal
 * state for reuse. It ensures that threads access the critical section in a
 * predefined order, making it useful in parallel data processing pipelines.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#include "genie/util/ordered_lock.h"

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------
void OrderedLock::Wait(const size_t id) {
  std::unique_lock lock(m_);
  if (id == counter_) {
    return;
  }
  cond_var_.wait(lock, [&]() -> bool { return id == counter_; });
}

// -----------------------------------------------------------------------------
void OrderedLock::Finished(const size_t length) {
  {
    std::unique_lock lock(m_);
    counter_ += length;
  }
  cond_var_.notify_all();
}

// -----------------------------------------------------------------------------
OrderedLock::OrderedLock() : counter_(0) {}

// -----------------------------------------------------------------------------
void OrderedLock::Reset() { counter_ = 0; }

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
