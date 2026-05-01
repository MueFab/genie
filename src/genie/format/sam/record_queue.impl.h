/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_RECORD_QUEUE_IMPL_H_
#define SRC_GENIE_FORMAT_SAM_RECORD_QUEUE_IMPL_H_

// -----------------------------------------------------------------------------

#include <vector>
#include "genie/format/sam/record_queue.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam {

// -----------------------------------------------------------------------------

template <typename RecordType, typename CompareFun, typename CompleteFun>
void RecordQueue<RecordType, CompareFun, CompleteFun>::Add(
    const RecordType& rec) {
  waiting_records_.push(rec);
}

// -----------------------------------------------------------------------------

template <typename RecordType, typename CompareFun, typename CompleteFun>
std::vector<RecordType>
RecordQueue<RecordType, CompareFun, CompleteFun>::CompleteUntil(
    CompleteFun fun) {
  std::vector<RecordType> ret;
  while (!waiting_records_.empty() && fun(waiting_records_.top())) {
    ret.push_back(waiting_records_.top());
    waiting_records_.pop();
  }
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_RECORD_QUEUE_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
