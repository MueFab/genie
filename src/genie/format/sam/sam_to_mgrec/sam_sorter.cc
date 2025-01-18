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
#include <mutex>
#include <thread>
#include <vector>

#include "genie/util/runtime_exception.h"

namespace genie::format::sam::sam_to_mgrec {

/** compare function for open reads **/
bool CmpOpen::operator()(const std::vector<SamRecord>& a,
                         const std::vector<SamRecord>& b) const {
  UTILS_DIE_IF(a.empty() || b.empty(), "emtpy sam read in open list");
  return a.at(a.size() - 1).mate_pos_ > b.at(b.size() - 1).mate_pos_;
}

// -----------------------------------------------------------------------------

/** compare function for complete reads **/
bool CmpCompete::operator()(const std::vector<SamRecord>& a,
                            const std::vector<SamRecord>& b) const {
  UTILS_DIE_IF(a.empty() || b.empty(), "emtpy sam read in closed list");
  return a.at(0).mate_pos_ > b.at(0).mate_pos_;
}

// -----------------------------------------------------------------------------

SamSorter::SamSorter(const uint32_t max_waiting_distance)
    : open_queries_(CmpOpen()),
      completed_queries_(CmpCompete()),
      max_distance_(max_waiting_distance) {}

// -----------------------------------------------------------------------------

/** gets the smallest primary alignment position of the open list or maximum **/
void SamSorter::set_new_alignment_pos() {
  if (!open_queries_.empty()) {
    UTILS_DIE_IF(primary_alignment_positions_.size() > open_queries_.size(),
                 "too many positions");
    UTILS_DIE_IF(primary_alignment_positions_.size() > open_queries_.size(),
                 "not enough positions");
    cur_alignment_position_ =
        *std::min_element(primary_alignment_positions_.begin(),
                          primary_alignment_positions_.end());
  } else {
    cur_alignment_position_ = std::numeric_limits<int64_t>::max();
  }
}

// -----------------------------------------------------------------------------

/** checks if cur query is allowed on queries or needs to be put on completed
 * **/
void SamSorter::try_appending_to_queries(
    const std::vector<SamRecord>& cur_query) {
  /** cur_query is complete, try appending to queries **/
  if (cur_alignment_position_ >= cur_query.at(0).mate_pos_) {
    /** allowed to append to queries **/
    queries_.emplace_back();
    queries_.back() = cur_query;
  } else {
    /** not allowed to append to queries, append to complete_reads **/
    completed_queries_.push(cur_query);
  }
}

// -----------------------------------------------------------------------------

bool SamSorter::IsReadComplete(const std::vector<SamRecord>& read) {
  return read.at(read.size() - 1).mate_pos_ == read.at(0).mate_pos_;
}

// -----------------------------------------------------------------------------

void SamSorter::AddSamRead(const SamRecord& record) {
  /** this part checks if the top element on open_queries can be removed,
   * because it's mate was not found **/
  while (!open_queries_.empty() &&
         record.mate_pos_ >
             open_queries_.top().at(open_queries_.top().size() - 1).mate_pos_) {
    try_appending_to_queries(open_queries_.top());
    primary_alignment_positions_.remove(
        static_cast<int>(open_queries_.top().at(0).mate_pos_));
    open_queries_.pop();
  }

  /** this part checks where to put the new record **/
  if (static_cast<uint64_t>(record.mate_pos_) >
      static_cast<uint64_t>(record.mate_pos_) +
          static_cast<uint64_t>(max_distance_)) {
    /** mate is to far away, record gets treated as complete **/
    try_appending_to_queries({record});
  } else {
    /** here the record is not complete yet **/
    /** get all candidates that could be a match for new record **/
    std::vector<std::vector<SamRecord>> candidate_reads;
    auto candidate_pos =
        open_queries_.empty()
            ? 0
            : open_queries_.top().at(open_queries_.top().size() - 1).mate_pos_;
    while (!open_queries_.empty() &&
           open_queries_.top().at(open_queries_.top().size() - 1).mate_pos_ ==
               candidate_pos) {
      candidate_reads.emplace_back();
      candidate_reads.back() = open_queries_.top();
      open_queries_.pop();
    }

    /** check if new record belongs to any query in candidates **/
    for (auto& candidate : candidate_reads) {
      if (record.qname_ == candidate.at(0).qname_) {
        std::vector<SamRecord> cur_query = candidate;
        cur_query.push_back(record);
        if (IsReadComplete(cur_query)) {
          try_appending_to_queries(cur_query);
          primary_alignment_positions_.remove(
              static_cast<int>(cur_query.at(0).mate_pos_));
          candidate_pos = 1;
        } else {
          /** query is still not complete **/
          open_queries_.push(cur_query);
        }
      } else {
        open_queries_.push(candidate);
      }
    }

    /** new record didn't match any candidate **/
    if (candidate_pos != 1) {
      if (record.mate_pos_ <= record.mate_pos_) {  // TODO(sophie): incorrect
        open_queries_.push({record});
        primary_alignment_positions_.push_back(
            static_cast<int>(record.mate_pos_));
      } else {
        try_appending_to_queries({record});
      }
    }
  }
  set_new_alignment_pos();

  /** check if any completed queries can be put to queries **/
  while (!completed_queries_.empty() &&
         cur_alignment_position_ >= completed_queries_.top().at(0).mate_pos_) {
    queries_.emplace_back();
    queries_.back() = completed_queries_.top();
    completed_queries_.pop();
  }
}

// -----------------------------------------------------------------------------

std::vector<std::vector<SamRecord>> SamSorter::GetQueries() {
  std::vector<std::vector<SamRecord>> ret = queries_;
  queries_.clear();
  return ret;
}

// -----------------------------------------------------------------------------

void SamSorter::EndFile() {
  /** mark all records on open list as complete **/
  while (!open_queries_.empty()) {
    try_appending_to_queries(open_queries_.top());
    primary_alignment_positions_.remove(
        static_cast<int>(open_queries_.top().at(0).mate_pos_));
    open_queries_.pop();
  }

  /** append everything from complete_queries to queries **/
  while (!completed_queries_.empty()) {
    queries_.emplace_back();
    queries_.back() = completed_queries_.top();
    completed_queries_.pop();
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam::sam_to_mgrec

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
