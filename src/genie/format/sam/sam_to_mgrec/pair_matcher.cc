/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/sam/sam_to_mgrec/pair_matcher.h"

#include <limits>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam::sam_to_mgrec {

// -----------------------------------------------------------------------------

bool CmpMatePos::operator()(const SamRecord& a, const SamRecord& b) const {
  return a.mate_pos_ > b.mate_pos_;
}

// -----------------------------------------------------------------------------

void PairMatcher::RemovePosition(const uint64_t pos) {
  const auto it = unmatched_positions_.find(pos);
  UTILS_DIE_IF(it == unmatched_positions_.end(), "Position not in index");
  it->second -= 1;
  if (it->second == 0) {
    unmatched_positions_.erase(it);
  }
}

// -----------------------------------------------------------------------------

void PairMatcher::AddUnmatchedRead(const SamRecord& rec) {
  unmatched_positions_[rec.pos_] += 1;
  unmatched_pairs_.emplace(rec);
}

// -----------------------------------------------------------------------------

[[nodiscard]] uint64_t PairMatcher::GetLowestUnmatchedPosition() const {
  if (!unmatched_positions_.empty()) {
    return unmatched_positions_.begin()->first;
  }
  return std::numeric_limits<uint64_t>::max();
}

// -----------------------------------------------------------------------------

[[nodiscard]] std::vector<SamRecord> PairMatcher::Abandon(const uint64_t pos) {
  std::vector<SamRecord> abandoned;
  while (!unmatched_pairs_.empty() && unmatched_pairs_.top().mate_pos_ < pos) {
    RemovePosition(unmatched_pairs_.top().pos_);
    abandoned.emplace_back(unmatched_pairs_.top());
    unmatched_pairs_.pop();
  }
  return abandoned;
}

// -----------------------------------------------------------------------------

[[nodiscard]] std::optional<SamRecord> PairMatcher::Match(
    const SamRecord& record) {
  std::vector<SamRecord> candidate_reads;
  std::optional<SamRecord> mate = std::nullopt;
  while (!unmatched_pairs_.empty() &&
         unmatched_pairs_.top().mate_pos_ == record.pos_) {
    if (unmatched_pairs_.top().qname_ == record.qname_) {
      mate = unmatched_pairs_.top();
      unmatched_pairs_.pop();
      break;
    }
    candidate_reads.emplace_back(unmatched_pairs_.top());
    unmatched_pairs_.pop();
  }

  // Put back any non-matching candidates
  for (auto& c : candidate_reads) {
    unmatched_pairs_.emplace(std::move(c));
  }

  if (mate) {
    RemovePosition(mate->pos_);
  }

  return mate;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam::sam_to_mgrec

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
