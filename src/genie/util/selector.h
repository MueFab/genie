/**
 * @file selector.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the Selector class template for routing data between branches.
 *
 * This file contains the declaration of the `Selector` class template, which serves as a
 * multiplexer for directing data from one source to multiple branches based on a defined
 * operation. It combines the functionality of a `Drain` and a `Source` to act as an intermediary
 * in a data processing pipeline, routing incoming data to specific outputs.
 *
 * @details The `Selector` class is used in scenarios where data needs to be split and directed
 * to different processing branches based on a given condition or operation. It supports dynamic
 * addition and configuration of branches, allowing for flexible data flow management. The class
 * template is parameterized by the input type (`Tin`) and output type (`Tout`).
 */

#ifndef SRC_GENIE_UTIL_SELECTOR_H_
#define SRC_GENIE_UTIL_SELECTOR_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>
#include <functional>
#include <vector>
#include "genie/util/drain.h"
#include "genie/util/selector-head.h"
#include "genie/util/selector-tail.h"
#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief A multiplexer class template for directing data to multiple branches.
 *
 * The `Selector` class is a generic template that allows data of type `Tin` to be
 * selectively routed to multiple branches. It inherits from `Drain<Tin>` to consume
 * data and `Source<Tout>` to produce data, acting as an intermediary in a data pipeline.
 * It uses a custom operation to determine which branch the data should be directed to.
 *
 * @tparam Tin The type of the incoming data.
 * @tparam Tout The type of the outgoing data.
 */
template <typename Tin, typename Tout>
class Selector : public genie::util::Drain<Tin>, public genie::util::Source<Tout> {
 private:
    SelectorHead<Tin> head;   //!< @brief Manages the input branches for the selector.
    SelectorTail<Tout> tail;  //!< @brief Manages the output branches for the selector.

 public:
    /**
     * @brief Constructs a `Selector` object with no branches.
     *
     * Initializes an empty `Selector` object, which can be configured later
     * using the `addBranch()` or `setBranch()` methods to define the input
     * and output branches.
     */
    Selector() = default;

    /**
     * @brief Adds a new branch to the selector.
     *
     * This function adds a new input-output branch to the `Selector`. Each branch
     * consists of a `Drain<Tin>*` (input) and a `Source<Tout>*` (output). The new
     * branch will be appended to the list of branches managed by the selector.
     *
     * @param entry Pointer to the `Drain` representing the input branch.
     * @param out Pointer to the `Source` representing the output branch.
     */
    void addBranch(genie::util::Drain<Tin>* entry, genie::util::Source<Tout>* out);

    /**
     * @brief Sets a specific branch at the given index.
     *
     * This function updates the branch at the specified index with a new input-output
     * pair. If the index is out of bounds, it will resize the branch list to accommodate
     * the new entry.
     *
     * @param entry Pointer to the `Drain` representing the input branch.
     * @param out Pointer to the `Source` representing the output branch.
     * @param index The index at which to set the new branch.
     */
    void setBranch(genie::util::Drain<Tin>* entry, genie::util::Source<Tout>* out, size_t index);

    /**
     * @brief Sets the operation for determining the routing of data.
     *
     * This function sets the routing operation that determines which branch the
     * incoming data should be directed to. The operation is a `std::function`
     * that takes a reference to the incoming data and returns the index of the
     * branch to which the data should be routed.
     *
     * @param s The operation used for routing data to the branches.
     */
    void setOperation(std::function<size_t(const Tin& t)> s);

    /**
     * @brief Routes incoming data to the appropriate branch based on the operation.
     *
     * This function overrides the `flowIn` method from the `Drain` interface and
     * directs the incoming data to the appropriate branch as determined by the
     * configured operation. It uses the section identifier (`id`) to track the
     * data flow and ensure proper routing.
     *
     * @param t The incoming data to be routed.
     * @param id The section identifier for tracking data flow.
     */
    void flowIn(Tin&& t, const util::Section& id) override;

    /**
     * @brief Signals the end of data for all branches.
     *
     * This function overrides the `flushIn` method from the `Drain` interface
     * and propagates the end-of-data signal to all branches, ensuring that
     * all downstream components are notified.
     *
     * @param pos Reference to the position counter to be updated.
     */
    void flushIn(uint64_t& pos) override;

    /**
     * @brief Skips a section of data for all branches.
     *
     * This function overrides the `skipIn` method from the `Drain` interface and
     * skips the specified section of data for all branches, ensuring that the
     * state of the selector remains consistent.
     *
     * @param id The section identifier for the data to be skipped.
     */
    void skipIn(const Section& id) override;

    /**
     * @brief Sets the drain for the tail of the selector.
     *
     * This function sets the downstream `Drain` for the tail of the selector,
     * which will receive the processed output data.
     *
     * @param d Pointer to the `Drain` to be set as the downstream output.
     */
    void setDrain(genie::util::Drain<Tout>* d) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/selector.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SELECTOR_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
