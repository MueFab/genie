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

struct CmpOpen {
  bool operator()(const std::vector<SamRecord>& a,
                  const std::vector<SamRecord>& b) const;
};

struct CmpCompete {
  bool operator()(const std::vector<SamRecord>& a,
                  const std::vector<SamRecord>& b) const;
};

/**
 * @brief
 */
class SamSorter {
  /// Queries that can be returned at any moment
  std::vector<std::vector<SamRecord>> queries_;
  /// Automatically sorted by p_next because priority queue
  std::priority_queue<std::vector<SamRecord>,
                      std::vector<std::vector<SamRecord>>, CmpOpen>
      open_queries_;
  /// Automatically sorted by position of primary alignment as priority queue
  std::priority_queue<std::vector<SamRecord>,
                      std::vector<std::vector<SamRecord>>, CmpCompete>
      completed_queries_;
  /// Mapping position of the next ready query
  uint64_t cur_alignment_position_ = 0;
  /// All positions of primary alignments
  std::list<int> primary_alignment_positions_;
  /// Maximal allowed distance of two mates
  uint32_t max_distance_;

  /**
   * @brief This functions checks what the position of the record that needs
   * to be next is. If there is no record we are waiting on this number is
   * set to maximum uint64 value.
   */
  void set_new_alignment_pos();

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
  void try_appending_to_queries(const std::vector<SamRecord>& cur_query);

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
