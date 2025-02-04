/**
 * Copyright 2018-2024 The Genie Authors.
 * @file ordered_lock.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the OrderedLock class for enforcing execution order in
 * multithreaded environments.
 *
 * This file contains the declaration of the `OrderedLock` class, which is
 * designed to synchronize multiple threads and ensure that they execute in a
 * specified order. It uses a counter-based approach to track the order of
 * execution and condition variables to block or release threads based on their
 * assigned order.
 *
 * @details The `OrderedLock` class is useful in scenarios where concurrent
 * threads need to access shared resources in a deterministic sequence, such as
 * in parallel data processing pipelines. The class provides methods for waiting
 * until a thread's turn is reached, marking a thread as finished, and resetting
 * the internal state for reuse.
 */

#ifndef SRC_GENIE_UTIL_ORDERED_LOCK_H_
#define SRC_GENIE_UTIL_ORDERED_LOCK_H_

// -----------------------------------------------------------------------------

#include <condition_variable>  //NOLINT
#include <mutex>               //NOLINT

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Synchronizes concurrent threads to enforce a specific execution order.
 *
 * The `OrderedLock` class provides a mechanism to control the execution order
 * of multiple threads based on their unique identifiers. It uses a counter to
 * track which thread should execute next and employs a
 * `std::condition_variable` to block threads until their turn is reached. This
 * ensures that threads access the critical section in a predefined order.
 */
class OrderedLock {
  /// Tracks the thread identifier currently allowed
  size_t counter_;

  /// Mutex for protecting the internal state.
  std::mutex m_;

  /// Condition variable for managing thread synchronization.
  std::condition_variable cond_var_;

 public:
  /**
   * @brief Constructs an `OrderedLock` object with an initial state.
   *
   * Initializes the counter to zero, indicating that the first thread (ID =
   * 0) is allowed to execute. The internal state can be Reset using the
   * `Reset()` method.
   */
  OrderedLock();

  /**
   * @brief Waits until the current thread is allowed to execute.
   *
   * This function blocks the calling thread until its unique identifier `id`
   * matches the current value of the `counter`. This ensures that threads
   * access the critical section in the order of their IDs. When a thread
   * finishes, it should call `finished()` to allow the next thread to
   * proceed.
   *
   * @param id The unique identifier of the calling thread.
   */
  void Wait(size_t id);

  /**
   * @brief Marks the current thread as finished and triggers the next thread.
   *
   * This function increments the `counter` by the specified `length`,
   * indicating that the thread has completed its execution. It then notifies
   * waiting threads to check if they are now allowed to execute.
   *
   * @param length The number of threads or blocks to skip in the execution
   * order.
   */
  void Finished(size_t length);

  /**
   * @brief Resets the internal state of the lock.
   *
   * This function resets the `counter` to zero, allowing the first thread (ID
   * = 0) to execute. It is useful when reusing the `OrderedLock` object in a
   * new context or pipeline.
   */
  void Reset();
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_ORDERED_LOCK_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
