/**
 * Copyright 2018-2024 The Genie Authors.
 * @file selector.impl.h
 * @brief Implementation of the Selector class template for managing data flow
 * through branches.
 *
 * This file contains the implementation of the Selector class template, which
 * is responsible for managing the flow of data through multiple branches. It
 * includes methods for adding and setting branches, defining a custom selection
 * function, and managing the flow of data through the branches.
 *
 * @details The Selector class template provides mechanisms to add and set
 * branches, route data to the appropriate branch, Flush data through all
 * branches, and skip sections of data. It supports operations such as adding
 * branches, setting a specific branch at an index, and defining a custom
 * selection function to determine the target branch for the data.
 *
 * @tparam Tin The type of the input data being routed to the branches.
 * @tparam Tout The type of the output data being processed by the branches.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_SELECTOR_IMPL_H_
#define SRC_GENIE_UTIL_SELECTOR_IMPL_H_

// -----------------------------------------------------------------------------

#include <utility>

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::AddBranch(Drain<Tin>* entry, Source<Tout>* out) {
  head_.Add(entry);
  tail_.AddMod();
  out->SetDrain(&tail_);
}

// -----------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::SetBranch(Drain<Tin>* entry, Source<Tout>* out,
                                    size_t index) {
  head_.Set(entry, index);
  out->SetDrain(&tail_);
}

// -----------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::SetOperation(std::function<size_t(const Tin& t)> s) {
  head_.SetOperation(s);
}

// -----------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::FlowIn(Tin&& t, const Section& id) {
  head_.FlowIn(std::move(t), id);
}

// -----------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::FlushIn(uint64_t& pos) {
  head_.FlushIn(pos);
}

// -----------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::SkipIn(const Section& id) {
  head_.SkipIn(id);
}

// -----------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::SetDrain(Drain<Tout>* d) {
  tail_.SetDrain(d);
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SELECTOR_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
