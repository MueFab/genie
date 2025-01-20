/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/sam/sam_to_mgrec/sam_sorter.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <optional>
#include <vector>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam::sam_to_mgrec {

// -----------------------------------------------------------------------------

bool CmpOpen::operator()(const SamRecord& a,
                         const SamRecord& b) const {
  return a.mate_pos_ > b.mate_pos_;
}

// -----------------------------------------------------------------------------

bool CmpComplete::operator()(const SamRecordPair& a,
                             const SamRecordPair& b) const {
  UTILS_DIE_IF(a.empty() || b.empty(), "emtpy sam read in closed list");
  return a.front().mate_pos_ > b.front().mate_pos_;
}

// -----------------------------------------------------------------------------

SamSorter::SamSorter(const uint32_t max_waiting_distance)
    : unmatched_pairs_(CmpOpen()),
      matched_pairs_(CmpComplete()),
      max_distance_(max_waiting_distance) {}

// -----------------------------------------------------------------------------

void SamSorter::SetNewAlignmentPos() {
  if (!unmatched_pairs_.empty()) {
    UTILS_DIE_IF(primary_alignment_positions_.size() > unmatched_pairs_.size(),
                 "too many positions");
    UTILS_DIE_IF(primary_alignment_positions_.size() > unmatched_pairs_.size(),
                 "not enough positions");
    cur_alignment_position_ =
        *std::min_element(primary_alignment_positions_.begin(),
                          primary_alignment_positions_.end());
  } else {
    cur_alignment_position_ = std::numeric_limits<int64_t>::max();
  }
}

// -----------------------------------------------------------------------------

void SamSorter::FinishQuery(const std::vector<SamRecord>& cur_query) {
  // cur_query is complete, try appending to queries
  if (cur_alignment_position_ >= cur_query.front().mate_pos_) {
    /** allowed to append to queries **/
    sorted_pairs_.emplace_back();
    sorted_pairs_.back() = cur_query;
  } else {
    /** not allowed to append to queries, append to complete_reads **/
    matched_pairs_.push(cur_query);
  }
}

// -----------------------------------------------------------------------------

bool SamSorter::IsReadComplete(const std::vector<SamRecord>& read) {
  return read.back().mate_pos_ == read.front().mate_pos_;
}

void SamSorter::FinishOrphanedReads(const uint64_t current_pos) {
  // We have already passed the position where the mate was expected...
  // This means that the mate is not present in the file.
  while (!unmatched_pairs_.empty() &&
         current_pos > unmatched_pairs_.top().mate_pos_) {
    FinishQuery({unmatched_pairs_.top()});
    unmatched_pairs_.pop();
  }
}

// -----------------------------------------------------------------------------

uint64_t unsigned_distance(uint64_t a, uint64_t b) {
  return (a > b) ? (a - b) : (b - a);
}

void SamSorter::AddUnmappedPair(const SamRecord& rec) {
  // TODO
}

std::optional<SamRecord> SamSorter::MatchPair(const SamRecord& record) {
  /** here the record is not complete yet **/
  /** get all candidates that could be a match for new record **/
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

  return mate;
}

void SamSorter::AddSamRead(const SamRecord& record) {
  // Fully unmapped pair
  if ((!record.IsPaired() && record.IsUnmapped()) ||
      (record.IsPaired() && record.IsUnmapped() && record.IsMateUnmapped())) {
    AddUnmappedPair(record);
    return;
  }

  FinishOrphanedReads(record.pos_);

  // No mate to wait for
  if (!record.IsPaired() || !record.IsPrimary()) {
    FinishQuery({record});
    return;
  }

  // There is a mate, but it is too far away to wait for it.
  if (record.mate_rid_ != record.rid_ ||
      unsigned_distance(record.pos_, record.mate_pos_) > max_distance_) {
    FinishQuery({record});
    return;
  }

  if (record.mate_pos_ > record.pos_) {
    // Impossible that we have already seen the mate, it is at higher position
    unmatched_pairs_.emplace(record);
    return;
  }
  auto mate = MatchPair(record);
  if (!mate) {
    if (record.mate_pos_ < record.pos_) {
      // No mate but we should have seen it... Orphan.
      FinishQuery({record});
      return;
    }
    // Mate maps to same position. There is still hope.
    unmatched_pairs_.emplace(record);
    return;
  }

  // Mate found
  FinishQuery({*mate, record});
  return;
  
  SetNewAlignmentPos();

  /** check if any completed queries can be put to queries **/
  while (!matched_pairs_.empty() &&
         cur_alignment_position_ >= matched_pairs_.top().front().mate_pos_) {
    sorted_pairs_.emplace_back();
    sorted_pairs_.back() = matched_pairs_.top();
    matched_pairs_.pop();
  }
}

// -----------------------------------------------------------------------------

std::vector<std::vector<SamRecord>> SamSorter::GetQueries() {
  return std::move(sorted_pairs_);
}

// -----------------------------------------------------------------------------

void SamSorter::EndFile() {
  /** mark all records on open list as complete **/
  while (!unmatched_pairs_.empty()) {
    FinishQuery(unmatched_pairs_.top());
    unmatched_pairs_.pop();
  }

  /** append everything from complete_queries to queries **/
  while (!matched_pairs_.empty()) {
    sorted_pairs_.emplace_back(matched_pairs_.top());
    matched_pairs_.pop();
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam::sam_to_mgrec

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
