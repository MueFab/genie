/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_PAIR_MATCHER_H_
#define SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_PAIR_MATCHER_H_

// -----------------------------------------------------------------------------

#include <map>
#include <optional>
#include <queue>
#include <vector>

#include "genie/format/sam/sam_record.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam::sam_to_mgrec {

// -----------------------------------------------------------------------------

struct CmpMatePos {
  bool operator()(const SamRecord& a, const SamRecord& b) const;
};

/**
 * @brief Class to match pairs of reads
 */
class PairMatcher {
  /// Reads waiting for their mate
  std::priority_queue<SamRecord, std::vector<SamRecord>, CmpMatePos>
      unmatched_pairs_;

  /// Positions of the incomplete pairs that are waiting
  std::map<uint64_t, uint64_t> unmatched_positions_;

  /**
   * @brief Remove a position from the index unmatched_positions_
   * @param pos Position to remove
   */
  void RemovePosition(uint64_t pos);

 public:
  /**
   * @brief Add a read to the set of unmatched reads
   * @param rec Read to add
   */
  void AddUnmatchedRead(const SamRecord& rec);

  /**
   * @brief Get the lowest mapping position of all unmatched reads
   * @return The lowest unmatched position
   */
  [[nodiscard]] uint64_t GetLowestUnmatchedPosition() const;

  /**
   * @brief Abandon all reads whose mate is mapped before a certain position,
   * i.e. give up on matching them
   * @param pos Position to abandon reads before
   * @return The abandoned reads
   */
  [[nodiscard]] std::vector<SamRecord> Abandon(uint64_t pos);

  /**
   * @brief Try to match a read to its mate with lower mapping position.
   * If the mate is found, it is removed from the matcher and returned.
   * @param record Read to match
   * @return The mate of the read if found.
   */
  [[nodiscard]] std::optional<SamRecord> Match(const SamRecord& record);
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam::sam_to_mgrec

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_PAIR_MATCHER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
