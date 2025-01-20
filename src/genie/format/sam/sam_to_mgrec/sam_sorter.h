/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_SAM_SORTER_H_
#define SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_SAM_SORTER_H_

// -----------------------------------------------------------------------------

#include <list>
#include <queue>
#include <vector>

#include "genie/format/sam/sam_to_mgrec/sam_record.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam::sam_to_mgrec {

// -----------------------------------------------------------------------------

using SamRecordPair = std::vector<SamRecord>;

struct CmpOpen {
  bool operator()(const SamRecord& a,
                  const SamRecord& b) const;
};

struct CmpComplete {
  bool operator()(const SamRecordPair& a, const SamRecordPair& b) const;
};

/**
 * @brief
 */
class SamSorter {
  /// Reads waiting for their mate
  std::priority_queue<SamRecord, std::vector<SamRecord>, CmpOpen>
      unmatched_pairs_;
  /// Reads that are already matched but have to wait as there is still
  /// an incomplete pair before them
  std::priority_queue<SamRecordPair, std::vector<SamRecordPair>, CmpComplete>
      matched_pairs_;
  /// Output buffer for already sorted pairs
  std::vector<SamRecordPair> sorted_pairs_;

  /// Mapping position of the next ready query
  uint64_t cur_alignment_position_ = 0;
  /// Maximal allowed distance of two mates. If exceeded, the pair is split
  uint32_t max_distance_;

  void FinishOrphanedReads(uint64_t current_pos);

  void AddUnmappedPair(const SamRecord& rec);

  /**
   * @brief This functions checks what the position of the record that needs
   * to be next is. If there is no record we are waiting on this number is
   * set to maximum uint64 value.
   */
  void SetNewAlignmentPos();

  /**
   * @brief This function checks if the record is ready to be added to the
   * queries. If it is, it is added to the queries and the function returns
   * true. If it is not ready, the function returns false.
   */
  static bool IsReadComplete(const std::vector<SamRecord>& read);

  /**
   * @brief This function takes cur_query and checks, if it's okay to append
   * it to queries. If we are still waiting for a record with smaller position,
   * then cur_query is appended to completed_queries.
   */
  void FinishQuery(const std::vector<SamRecord>& cur_query);

 public:
  /**
   * @brief
   * @param max_waiting_distance
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
  std::vector<std::vector<SamRecord>> GetQueries();

  /**
   * @brief Function to signal end of data
   * clears all member variables (open_queries and complete_queries)
   * and puts remaining records to queries
   */
  void EndFile();
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam::sam_to_mgrec

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_SAM_SORTER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
