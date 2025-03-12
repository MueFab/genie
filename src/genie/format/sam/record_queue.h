/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_RECORD_QUEUE_H_
#define SRC_GENIE_FORMAT_SAM_RECORD_QUEUE_H_

// -----------------------------------------------------------------------------

#include <queue>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::format::sam {

/**
 *
 * @tparam RecordType Records to queue and sort
 * @tparam CompareFun Function to compare two records.
 * Signature: bool fun (const RecordType &a, const RecordType &b);
 * Return true if a > b.
 * @tparam CompleteFun Function checking if the queue is completed until this
 * record. Signature: bool fun(const RecordType &a), return true if complete
 */
template <typename RecordType, typename CompareFun, typename CompleteFun>
class RecordQueue {
  /// A priority queue holding the record objects
  std::priority_queue<RecordType, std::vector<RecordType>, CompareFun>
      waiting_records_;

 public:
  /**
   * @brief Add a record to the queue.
   * @param rec Record to add
   */
  void Add(const RecordType& rec);

  /**
   * @brief Return the records at the beginning of the queue that are already
   * completed
   * @param fun Function to determine if a record is completed
   * @return Vector of completed records.
   */
  [[nodiscard]] std::vector<RecordType> CompleteUntil(CompleteFun fun);
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam

// -----------------------------------------------------------------------------

#include "genie/format/sam/record_queue.impl.h"

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_RECORD_QUEUE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
