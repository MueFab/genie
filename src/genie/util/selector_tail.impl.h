/**
 * Copyright 2018-2024 The Genie Authors.
 * @file selector_tail.impl.h
 * @brief Implementation of the SelectorTail class template for managing data
 * flow in a pipeline.
 *
 * This file contains the implementation of the SelectorTail class template,
 * which is responsible for managing the flow of data through a pipeline. It
 * includes methods for adding modules, flowing data in, flushing data, and
 * skipping sections.
 *
 * @details The SelectorTail class template provides mechanisms to handle
 * multiple modules, ensuring that data is processed correctly through the
 * pipeline. It supports operations such as adding modules, flowing data into
 * the pipeline, flushing data when all modules have finished processing, and
 * skipping sections based on specific conditions.
 *
 * @tparam Tout The type of the data being processed in the pipeline.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_SELECTOR_TAIL_IMPL_H_
#define SRC_GENIE_UTIL_SELECTOR_TAIL_IMPL_H_

// -----------------------------------------------------------------------------

#include <utility>

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

template <typename Tout>
SelectorTail<Tout>::SelectorTail() : dry_ctr_(0), mod_num_(0) {}

// -----------------------------------------------------------------------------

template <typename Tout>
void SelectorTail<Tout>::AddMod() {
  mod_num_++;
}

// -----------------------------------------------------------------------------

template <typename Tout>
void SelectorTail<Tout>::FlowIn(Tout&& t, const Section& id) {
  Source<Tout>::FlowOut(std::move(t), id);
}

// -----------------------------------------------------------------------------

template <typename Tout>
void SelectorTail<Tout>::FlushIn(uint64_t& pos) {
  if (const size_t num = ++dry_ctr_; num == mod_num_) {
    // Output signal once every module contained finished
    dry_ctr_ = 0;
    Source<Tout>::FlushOut(pos);
  }
}

// -----------------------------------------------------------------------------

template <typename Tout>
void SelectorTail<Tout>::SkipIn(const Section& id) {
  if (id.strong_skip) {
    Source<Tout>::SkipOut(id);
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SELECTOR_TAIL_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
