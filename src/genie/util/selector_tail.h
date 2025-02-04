/**
 * Copyright 2018-2024 The Genie Authors.
 * @file selector_tail.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the SelectorTail class template for managing the tail
 * of a data selector.
 *
 * This file contains the declaration of the `SelectorTail` class template,
 * which acts as the tail component of a data selector. It is responsible for
 * receiving data from multiple branches and forwarding it to a downstream
 * consumer. The class serves as both a `Source` and `Drain`, enabling it to
 * manage incoming and outgoing data in a synchronized manner.
 *
 * @details The `SelectorTail` class template is parameterized by the output
 * data type (`Tout`) and provides methods for adding branches, handling data
 * flow, and managing end-of-data signals. It maintains internal counters to
 * track the state of the branches and ensure proper synchronization.
 */

#ifndef SRC_GENIE_UTIL_SELECTOR_TAIL_H_
#define SRC_GENIE_UTIL_SELECTOR_TAIL_H_

// -----------------------------------------------------------------------------

#include "genie/util/source.h"

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Class template for managing the tail of a data selector.
 *
 * The `SelectorTail` class is responsible for receiving data from multiple
 * branches and managing the flow to a downstream consumer. It inherits from
 * both `Source<Tout>` and `Drain<Tout>`, allowing it to function as an
 * intermediary between different components in a data pipeline. The class
 * maintains counters to track the state of the branches and ensure that all
 * branches have completed their operations before propagating the end-of-data
 * signal.
 *
 * @tparam Tout The type of the outgoing data.
 */
template <typename Tout>
class SelectorTail final : public Source<Tout>, public Drain<Tout> {
  /// Counter for tracking the number of branches that have finished sending.
  size_t dry_ctr_;

  /// Total number of branches connected to this selector tail.
  size_t mod_num_;

 public:
  /**
   * @brief Constructs a `SelectorTail` object with no branches.
   *
   * Initializes a `SelectorTail` object with all counters set to zero.
   * The `addMod()` method should be used to add branches to the tail.
   */
  SelectorTail();

  /**
   * @brief Adds a new branch (module) to the selector tail.
   *
   * This function increments the total number of branches (`modNum`) that
   * the selector tail is expected to handle. It should be called each time
   * a new branch is connected to the selector tail.
   */
  void AddMod();

  /**
   * @brief Receives data from an incoming branch and forwards it downstream.
   *
   * This function overrides the `flowIn` method from the `Drain` interface
   * and forwards the incoming data to the connected downstream component. It
   * uses the section identifier (`id`) to track the data flow and ensure
   * synchronization.
   *
   * @param t The incoming data to be forwarded.
   * @param id The section identifier for tracking data flow.
   */
  void FlowIn(Tout&& t, const Section& id) override;

  /**
   * @brief Signals the end of data for a specific branch.
   *
   * This function overrides the `flushIn` method from the `Drain` interface
   * and updates the internal state to reflect that a branch has finished
   * sending data. When all branches have signaled the end of data, it
   * propagates the signal downstream.
   *
   * @param pos Reference to the position counter to be updated.
   */
  void FlushIn(uint64_t& pos) override;

  /**
   * @brief Skips a section of data for all branches.
   *
   * This function overrides the `skipIn` method from the `Drain` interface
   * and skips the specified section of data for all branches, ensuring that
   * the state of the selector tail remains consistent.
   *
   * @param id The section identifier for the data to be skipped.
   */
  void SkipIn(const Section& id) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#include "genie/util/selector_tail.impl.h"  // NOLINT

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SELECTOR_TAIL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
