/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/sam/sam_sorter.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <optional>
#include <utility>
#include <vector>

#include "genie/util/log.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam {

// -----------------------------------------------------------------------------

bool CmpPairPos::operator()(const SamRecordPair& a,
                                const SamRecordPair& b) const {
  return a.first.pos_ > b.first.pos_;
}


// -----------------------------------------------------------------------------

SamSorter::SamSorter(const uint32_t max_waiting_distance)
    : max_distance_(max_waiting_distance) {}

// -----------------------------------------------------------------------------

void SamSorter::FinishPair(const SamRecordPair& cur_query) {
  this->pair_queue_.Add(cur_query);
  for (auto& c : pair_queue_.CompleteUntil(
           CmpPairPosLess(pair_matcher_.GetLowestUnmatchedPosition()))) {
    pair_buffer_.emplace_back(std::move(c));
  }
}

// -----------------------------------------------------------------------------

uint64_t unsigned_distance(uint64_t a, uint64_t b) {
  return (a > b) ? (a - b) : (b - a);
}

void SamSorter::AddUnmappedPair(const SamRecord& rec) {
  auto result = this->unmapped_matcher_.AddSamRead(rec);
  if (result) {
    pair_buffer_.emplace_back(std::move(*result));
  }
}

void SamSorter::AddSamRead(const SamRecord& record) {
  // Fully unmapped pair
  if ((!record.IsPaired() && record.IsUnmapped()) ||
      (record.IsPaired() && record.IsUnmapped() && record.IsMateUnmapped())) {
    AddUnmappedPair(record);
    return;
  }

  // Finish reads where we saw the first mate but the second mate is missing
  // After this, the lowest unmatched mate_pos is >= record.pos_
  for (auto& orphan : pair_matcher_.Abandon(record.pos_)) {
    FinishPair({orphan, std::nullopt});
  }

  // Unpaired or Non-Primary: No mate exists
  if (!record.IsPaired() || !record.IsPrimary()) {
    FinishPair({record, std::nullopt});
    return;
  }

  // There is a mate, but it is too far away to wait for it.
  if (record.mate_rid_ != record.rid_ ||
      unsigned_distance(record.pos_, record.mate_pos_) > max_distance_) {
    FinishPair({record, std::nullopt});
    return;
  }

  // The mate will come later, we must wait for it
  if (record.mate_pos_ > record.pos_) {
    pair_matcher_.AddUnmatchedRead(record);
    return;
  }
  auto mate = pair_matcher_.Match(record);
  if (!mate) {
    // We have the second mate, but the first mate is missing
    if (record.mate_pos_ < record.pos_) {
      FinishPair({record, std::nullopt});
      return;
    }

    // Mate maps to the same position and may still appear
    pair_matcher_.AddUnmatchedRead(record);
    return;
  }

  FinishPair({*mate, record});
}

// -----------------------------------------------------------------------------

std::vector<SamRecordPair> SamSorter::GetPairs() {
  return std::move(pair_buffer_);
}

// -----------------------------------------------------------------------------

void SamSorter::Finish() {
  for (auto& p :
       this->pair_matcher_.Abandon(std::numeric_limits<uint64_t>::max())) {
    this->pair_queue_.Add({p, std::nullopt});
  }
  for (auto& c : pair_queue_.CompleteUntil(
           CmpPairPosLess(std::numeric_limits<uint64_t>::max()))) {
    this->pair_buffer_.emplace_back(std::move(c));
  }
  this->unmapped_matcher_.finish();
  if (auto result = this->unmapped_matcher_.GetAfterFinish()) {
    pair_buffer_.emplace_back(std::move(*result));
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
