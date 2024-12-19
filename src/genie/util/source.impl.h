/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_SOURCE_IMPL_H_
#define SRC_GENIE_UTIL_SOURCE_IMPL_H_

// -----------------------------------------------------------------------------

#include <utility>

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

template <typename Type>
void Source<Type>::FlowOut(Type&& t, const Section& id) {
  drain_->FlowIn(std::move(t), id);
}

// -----------------------------------------------------------------------------

template <typename Type>
void Source<Type>::FlushOut(uint64_t& pos) {
  drain_->FlushIn(pos);
}

// -----------------------------------------------------------------------------

template <typename Type>
void Source<Type>::SkipOut(const Section& id) {
  drain_->SkipIn(id);
}

// -----------------------------------------------------------------------------

template <typename Type>
void Source<Type>::SetDrain(Drain<Type>* d) {
  drain_ = d;
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SOURCE_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
