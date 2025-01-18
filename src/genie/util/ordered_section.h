/**
 * Copyright 2018-2024 The Genie Authors.
 * @file ordered_section.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the OrderedSection class for managing ordered execution
 * of code sections.
 *
 * This file contains the declaration of the `OrderedSection` class, which
 * ensures that specific sections of code are executed in a predefined order
 * using an `OrderedLock`. It provides a mechanism for protecting critical
 * sections using scoped locking and automatic release upon destruction of the
 * `OrderedSection` object.
 *
 * @details The `OrderedSection` class works in conjunction with the
 * `OrderedLock` class to enforce ordered execution of threads or data blocks.
 * It is designed to be used in a RAII (Resource Acquisition Is Initialization)
 * style, acquiring the lock when constructed and releasing it when destroyed.
 * This ensures safe and deterministic execution order of critical sections.
 */

#ifndef SRC_GENIE_UTIL_ORDERED_SECTION_H_
#define SRC_GENIE_UTIL_ORDERED_SECTION_H_

// -----------------------------------------------------------------------------

#include "genie/util/drain.h"
#include "genie/util/ordered_lock.h"

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Manages ordered execution of a code section using an `OrderedLock`.
 *
 * The `OrderedSection` class provides a scoped lock for protecting critical
 * sections of code in a specific execution order. It uses the `OrderedLock` to
 * control access to the protected section, ensuring that threads or data blocks
 * are executed in the order defined by their unique identifiers.
 *
 * @details The `OrderedSection` acquires the lock using a specified `id` upon
 * construction and releases it automatically when the object goes out of scope.
 * This ensures that the protected section is executed in a deterministic order,
 * preventing race conditions and ensuring correct synchronization between
 * concurrent threads.
 */
class OrderedSection {
  /// Pointer to the underlying `OrderedLock` object for synchronization.
  OrderedLock* lock_;

  /// The number of blocks/threads that this section encompasses.
  size_t length_;

 public:
  /**
   * @brief Constructs an `OrderedSection` and acquires the lock.
   *
   * This constructor attempts to acquire the specified `OrderedLock` using
   * the provided `id`. It blocks until the current thread or block is allowed
   * to execute based on the order enforced by the lock.
   *
   * @param lock Pointer to the underlying `OrderedLock` used for
   * synchronization.
   * @param id The section identifier used to determine the order of
   * execution.
   */
  OrderedSection(OrderedLock* lock, const Section& id);

  /**
   * @brief Destroys the `OrderedSection` and releases the lock.
   *
   * This destructor releases the lock, allowing the next thread or block in
   * the sequence to proceed. It ensures that the ordered execution is
   * maintained and that other threads can acquire the lock in their specified
   * order.
   */
  ~OrderedSection();
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_ORDERED_SECTION_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
