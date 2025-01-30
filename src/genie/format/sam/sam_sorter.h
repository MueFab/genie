/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_SAM_SORTER_H_
#define SRC_GENIE_FORMAT_SAM_SAM_SORTER_H_

// -----------------------------------------------------------------------------

#include <vector>

#include "genie/format/sam/pair_matcher.h"
#include "genie/format/sam/record_queue.h"
#include "genie/format/sam/sam_record.h"
#include "genie/format/sam/unmapped_record_matcher.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam {

// -----------------------------------------------------------------------------

struct CmpPairPos {
  bool operator()(const SamRecordPair& a, const SamRecordPair& b) const;
};

class CmpPairPosLess {
  uint64_t pos_;

 public:
  explicit CmpPairPosLess(const uint64_t pos) : pos_(pos) {}
  bool operator()(const SamRecordPair& a) const {
    return a.first.pos_ < pos_;
  }
};

using PairQueue =
    RecordQueue<SamRecordPair, CmpPairPos, CmpPairPosLess>;

/**
 * @brief Class to match pairs of reads
 */
class SamSorter {
  /// Unmatched records waiting for their mate
  PairMatcher pair_matcher_;

  /// Matched records that have to wait for an incomplete pair with a lower pos
  PairQueue pair_queue_;

  /// Output buffer for already finished pairs
  std::vector<SamRecordPair> pair_buffer_;

  /// Maximal allowed distance of two mates. If exceeded, the pair is split
  uint32_t max_distance_;

  /// Matcher for unmapped pairs
  UnmappedRecordMatcher unmapped_matcher_;

  /**
   * @brief Process a record that is part of a completely unmapped pair
   * @param rec Record to process
   */
  void AddUnmappedPair(const SamRecord& rec);

  /**
   * @brief Add a matched pair to the queue and finally output buffer
   * @param cur_query Pair to finish
   */
  void FinishPair(const SamRecordPair& cur_query);

 public:
  /**
   * @brief Construct a new SamSorter object
   * @param max_waiting_distance Maximal distance of two mates. If exceeded, the
   * mates are split into two records
   */
  explicit SamSorter(uint32_t max_waiting_distance);

  /**
   * @brief Function for sorting one Record.
   */
  void AddSamRead(const SamRecord&);

  /**
   * @brief Function for getting already sorted queries.
   * The queries data structure is also cleared afterward.
   */
  std::vector<SamRecordPair> GetPairs();

  /**
   * @brief Function to signal end of data
   * clears all member variables (open_queries and complete_queries)
   * and puts remaining records into output buffer
   */
  void Finish();
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_SAM_SORTER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
