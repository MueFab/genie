/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_MERGE_SORT_MERGER_IMPL_H_
#define SRC_GENIE_UTIL_MERGE_SORT_MERGER_IMPL_H_

// -----------------------------------------------------------------------------

#include <utility>
#include <vector>
#include "genie/util/merge_sort/merger.h"

// -----------------------------------------------------------------------------

namespace genie::util::merge_sort {

// -----------------------------------------------------------------------------

template <class T, typename SortFun>
Merger<T, SortFun>::Merger(SortFun sort_fun,
                           std::vector<std::unique_ptr<Chunk<T>>> chunks)
    : sort_fun_(sort_fun),
      chunks_(std::move(chunks)),
      cmp_(sort_fun_, chunks_),
      queue_(cmp_) {
  for (size_t i = 0; i < chunks_.size(); ++i) {
    if (!chunks_[i]->IsEmpty()) {
      queue_.push(i);
    }
  }
}

// -----------------------------------------------------------------------------

template <class T, typename SortFun>
bool Merger<T, SortFun>::IsEmpty() const {
  return queue_.empty();
}

// -----------------------------------------------------------------------------

template <class T, typename SortFun>
T Merger<T, SortFun>::Get() {
  const auto idx = queue_.top();
  auto record = chunks_[idx]->Get();
  chunks_[idx]->Pop();
  queue_.pop();
  if (!chunks_[idx]->IsEmpty()) {
    queue_.push(idx);
  }
  return record;
}

// -----------------------------------------------------------------------------

}  // namespace genie::util::merge_sort

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_MERGE_SORT_MERGER_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
