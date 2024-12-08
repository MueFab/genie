/**
 * Copyright 2018-2024 The Genie Authors.
 * @file selector_head.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the SelectorHead class template for managing data
 * routing in a selector.
 *
 * This file contains the declaration of the `SelectorHead` class template,
 * which acts as the head component of a data selector. It routes incoming data
 * to multiple downstream modules based on a user-defined selection operation.
 * The class serves as the input interface for a `Selector`, determining which
 * branch should receive each incoming data chunk.
 *
 * @details The `SelectorHead` class template is parameterized by the input data
 * type (`Tin`) and provides methods for configuring branches, setting the
 * selection operation, and handling data flow. It can be dynamically configured
 * to route data to different branches based on custom logic.
 */

#ifndef SRC_GENIE_UTIL_SELECTOR_HEAD_H_
#define SRC_GENIE_UTIL_SELECTOR_HEAD_H_

// -----------------------------------------------------------------------------

#include <functional>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Class template for managing the input head of a data selector.
 *
 * The `SelectorHead` class acts as the input head for a data selector,
 * determining which branch should receive each incoming data chunk. It inherits
 * from `Drain<Tin>` and routes incoming data to a dynamically configurable set
 * of branches using a custom selection operation.
 *
 * @tparam Tin The type of incoming data to be routed.
 */
template <typename Tin>
class SelectorHead : public Drain<Tin> {
  std::vector<Drain<Tin>*> mods_;  //!< @brief Stores the downstream modules
                                   //!< (branches) to which data is routed.
  std::function<size_t(const Tin& t)>
      select_;  //!< @brief The selection operation used to choose the
                //!< appropriate branch.

 protected:
  /**
   * @brief Routes data to the specified branch based on the selection
   * operation.
   *
   * This function sends the given data to the branch identified by `mod_id`.
   * It uses the `global_id` to track the section of the data being routed.
   * This method is called internally by `flowIn()` to perform the actual data
   * routing.
   *
   * @param t The data to be routed.
   * @param mod_id The index of the branch to which the data should be sent.
   * @param global_id The global section identifier for tracking the data
   * flow.
   */
  virtual void Route(Tin&& t, size_t mod_id, Section global_id);

  /**
   * @brief Default selection operation for routing data.
   *
   * This static function serves as the default selection operation if no
   * custom operation is provided. It returns a constant value (typically
   * zero), which routes all data to a single branch.
   *
   * @return The index of the default branch (zero).
   */
  static size_t DefaultSelect(const Tin&);

 public:
  /**
   * @brief Constructs a `SelectorHead` object with no branches.
   *
   * Initializes an empty `SelectorHead` object with the default selection
   * operation. Branches can be added later using the `add()` or `set()`
   * methods.
   */
  SelectorHead();

  /**
   * @brief Adds a new branch to the `SelectorHead`.
   *
   * This function appends a new downstream branch to the list of branches.
   * The branch is represented by a pointer to a `Drain<Tin>` module, which
   * will receive the data routed by the `SelectorHead`.
   *
   * @param mod Pointer to the `Drain` representing the new branch.
   */
  void Add(Drain<Tin>* mod);

  /**
   * @brief Sets a specific branch at the given index.
   *
   * This function updates the branch at the specified index with a new
   * `Drain<Tin>*`. If the index is out of bounds, it will resize the branch
   * list to accommodate the new entry.
   *
   * @param mod Pointer to the `Drain` representing the new branch.
   * @param index The index at which to set the new branch.
   */
  void Set(Drain<Tin>* mod, size_t index);

  /**
   * @brief Sets the operation for determining the branch for each data item.
   *
   * This function configures the selection operation used to decide which
   * branch should receive a given data item. The operation is a
   * `std::function` that takes a reference to the input data and returns the
   * index of the branch.
   *
   * @param s The selection operation used for branch routing.
   */
  void SetOperation(std::function<size_t(const Tin& t)> s);

  /**
   * @brief Routes incoming data to the appropriate branch based on the
   * selection operation.
   *
   * This function overrides the `flowIn` method from the `Drain` interface
   * and directs the incoming data to the appropriate branch as determined by
   * the configured operation.
   *
   * @param t The incoming data to be routed.
   * @param id The section identifier for tracking data flow.
   */
  void FlowIn(Tin&& t, const Section& id) override;

  /**
   * @brief Signals the end of data for all branches.
   *
   * This function overrides the `flushIn` method from the `Drain` interface
   * and propagates the end-of-data signal to all branches, ensuring that all
   * downstream components are notified.
   *
   * @param pos Reference to the position counter to be updated.
   */
  void FlushIn(uint64_t& pos) override;

  /**
   * @brief Skips a section of data for all branches.
   *
   * This function overrides the `skipIn` method from the `Drain` interface
   * and skips the specified section of data for all branches, ensuring that
   * the state of the selector remains consistent.
   *
   * @param id The section identifier for the data to be skipped.
   */
  void SkipIn(const Section& id) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#include "genie/util/selector_head.impl.h"

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SELECTOR_HEAD_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
