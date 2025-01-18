#include "genie/format/sam/sam_to_mgrec/sam_sorter.h"

#include <fcntl.h>

#include <algorithm>
#include <iostream>
#include <mutex>
#include <thread>

#include "genie/util/runtime_exception.h"

namespace genie::format::sam::sam_to_mgrec {

/** compare function for open reads **/
bool CmpOpen::operator()(const std::vector<SamRecord>& a,
                         const std::vector<sam_to_mgrec::SamRecord>& b) const {
  UTILS_DIE_IF(a.empty() || b.empty(), "emtpy sam read in open list");
  return a.at(a.size() - 1).mate_pos_ > b.at(b.size() - 1).mate_pos_;
}

// ---------------------------------------------------------------------------------------------------------------------

/** compare function for complete reads **/
bool CmpCompete::operator()(
    const std::vector<sam_to_mgrec::SamRecord>& a,
    const std::vector<sam_to_mgrec::SamRecord>& b) const {
  UTILS_DIE_IF(a.empty() || b.empty(), "emtpy sam read in closed list");
  return a.at(0).mate_pos_ > b.at(0).mate_pos_;
}

// ---------------------------------------------------------------------------------------------------------------------

SamSorter::SamSorter(uint32_t max_waiting_distance)
    : open_queries(CmpOpen()),
      completed_queries(CmpCompete()),
      max_distance(max_waiting_distance) {}

// ---------------------------------------------------------------------------------------------------------------------

/** gets the smallest primary alignment position of the open list or maximum **/
void SamSorter::set_new_alignment_pos() {
  if (!open_queries.empty()) {
    UTILS_DIE_IF(primary_alignment_positions.size() > open_queries.size(),
                 "too many positions");
    UTILS_DIE_IF(primary_alignment_positions.size() > open_queries.size(),
                 "not enough positions");
    cur_alignment_position = *std::min_element(
        primary_alignment_positions.begin(), primary_alignment_positions.end());
  } else {
    cur_alignment_position = std::numeric_limits<int64_t>::max();
  }
}

// ---------------------------------------------------------------------------------------------------------------------

/** checks if cur query is allowed on queries or needs to be put on completed
 * **/
void SamSorter::try_appending_to_queries(std::vector<SamRecord> cur_query) {
  /** cur_query is complete, try appending to queries **/
  if (cur_alignment_position >= cur_query.at(0).mate_pos_) {
    /** allowed to append to queries **/
    queries.emplace_back();
    queries.back() = cur_query;
  } else {
    /** not allowed to append to queries, append to complete_reads **/
    completed_queries.push(cur_query);
  }
}

// ---------------------------------------------------------------------------------------------------------------------

bool SamSorter::isReadComplete(std::vector<sam_to_mgrec::SamRecord> read) {
  return (read.at(read.size() - 1).mate_pos_ == read.at(0).mate_pos_);
}

// ---------------------------------------------------------------------------------------------------------------------

void SamSorter::addSamRead(const sam_to_mgrec::SamRecord& record) {
  /** this part checks if the top element on open_queries can be removed,
   * because it's mate was not found **/
  while (!open_queries.empty() &&
         record.mate_pos_ >
             open_queries.top().at(open_queries.top().size() - 1).mate_pos_) {
    try_appending_to_queries(open_queries.top());
    primary_alignment_positions.remove(open_queries.top().at(0).mate_pos_);
    open_queries.pop();
  }

  /** this part checks where to put the new record **/
  if ((uint64_t)record.mate_pos_ >
      (uint64_t)record.mate_pos_ + (uint64_t)max_distance) {
    /** mate is to far away, record gets treated as complete **/
    try_appending_to_queries({record});
  } else {
    /** here the record is not complete yet **/
    /** get all candidates that could be a match for new record **/
    std::vector<std::vector<SamRecord>> candidate_reads;
    auto candidate_pos =
        open_queries.empty()
            ? 0
            : open_queries.top().at(open_queries.top().size() - 1).mate_pos_;
    while (!open_queries.empty() &&
           open_queries.top().at(open_queries.top().size() - 1).mate_pos_ ==
               candidate_pos) {
      candidate_reads.emplace_back();
      candidate_reads.back() = open_queries.top();
      open_queries.pop();
    }

    /** check if new record belongs to any query in candidates **/
    for (auto& candidate : candidate_reads) {
      if (record.qname_ == candidate.at(0).qname_) {
        std::vector<SamRecord> cur_query = candidate;
        cur_query.push_back(record);
        if (isReadComplete(cur_query)) {
          try_appending_to_queries(cur_query);
          primary_alignment_positions.remove(cur_query.at(0).mate_pos_);
          candidate_pos = 1;
        } else {
          /** query is still not complete **/
          open_queries.push(cur_query);
        }
      } else {
        open_queries.push(candidate);
      }
    }

    /** new record didn't match any candidate **/
    if (candidate_pos != 1) {
      if (record.mate_pos_ <= record.mate_pos_) {
        open_queries.push({record});
        primary_alignment_positions.push_back(record.mate_pos_);
      } else {
        try_appending_to_queries({record});
      }
    }
  }
  set_new_alignment_pos();

  /** check if any completed queries can be put to queries **/
  while (!completed_queries.empty() &&
         cur_alignment_position >= completed_queries.top().at(0).mate_pos_) {
    queries.emplace_back();
    queries.back() = completed_queries.top();
    completed_queries.pop();
  }
}

std::vector<std::vector<sam_to_mgrec::SamRecord>> SamSorter::getQueries() {
  std::vector<std::vector<sam_to_mgrec::SamRecord>> ret = queries;
  queries.clear();
  return ret;
}

void SamSorter::endFile() {
  /** mark all records on open list as complete **/
  while (!open_queries.empty()) {
    try_appending_to_queries(open_queries.top());
    primary_alignment_positions.remove(open_queries.top().at(0).mate_pos_);
    open_queries.pop();
  }

  /** append everything from complete_queries to queries **/
  while (!completed_queries.empty()) {
    queries.emplace_back();
    queries.back() = completed_queries.top();
    completed_queries.pop();
  }
}

}  // namespace genie::format::sam::sam_to_mgrec

//