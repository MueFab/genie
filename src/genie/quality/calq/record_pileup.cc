/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/record_pileup.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

RecordPileup::RecordPileup()
    : min_pos_(static_cast<uint64_t>(-1)), max_pos_(static_cast<uint64_t>(0)) {}

// -----------------------------------------------------------------------------

std::string RecordPileup::preprocess(const std::string& read,
                                     const std::string& cigar) {
  std::string result;
  size_t count = 0;
  size_t read_pos = 0;

  for (const char cigar_pos : cigar) {
    if (std::isdigit(cigar_pos)) {
      count *= 10;
      count += cigar_pos - '0';
      continue;
    }
    if (cigar_pos == '(' || cigar_pos == '[') {
      continue;
    }
    if (GetAlphabetProperties(core::AlphabetId::kAcgtn).IsIncluded(cigar_pos) &&
        count == 0) {
      result += read[read_pos++];
      continue;
    }
    switch (cigar_pos) {
      case '=':
        for (size_t i = 0; i < count; ++i) {
          if (read_pos >= read.length()) {
            UTILS_THROW_RUNTIME_EXCEPTION(
                "CIGAR and Read lengths do not match");
          }
          result += read[read_pos++];
        }
        break;

      case '+':
      case ')':
        read_pos += count;
        break;

      case ']':
        break;

      case '-':
      case '*':
      case '/':
      case '%':
        for (size_t i = 0; i < count; ++i) {
          result += '0';
        }
        break;
      default:
        UTILS_THROW_RUNTIME_EXCEPTION("Unknown CIGAR character");
    }
    count = 0;
  }

  if (read_pos != read.length()) {
    UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
  }
  return result;
}

// -----------------------------------------------------------------------------

void RecordPileup::AddRecord(EncodingRecord& r) {
  NextRecord();

  records_.emplace_back(r);

  for (size_t i = 0; i < r.cigars.size(); ++i) {
    auto seq_processed = preprocess(r.sequences[i], r.cigars[i]);
    auto qual_processed = preprocess(r.quality_values[i], r.cigars[i]);

    this->min_pos_ = std::min(this->min_pos_, r.positions[i]);
    this->max_pos_ =
        std::max(this->max_pos_, r.positions[i] + seq_processed.length() - 1);

    preprocessed_qvalues_.back().emplace_back(std::move(qual_processed));
    preprocessed_sequences_.back().emplace_back(std::move(seq_processed));
  }
}

// -----------------------------------------------------------------------------

std::pair<std::string, std::string> RecordPileup::GetPileup(
    const uint64_t pos) const {
  UTILS_DIE_IF(pos < this->min_pos_ || pos > this->max_pos_,
               "Position out of range");

  std::string seqs, quals;

  for (uint64_t i = 0; i < this->records_.size(); ++i) {
    for (uint64_t read_i = 0; read_i < records_[i].positions.size(); ++read_i) {
      const auto pos_read = records_[i].positions[read_i];
      const auto& seq = preprocessed_sequences_[i][read_i];
      const auto& qual = preprocessed_qvalues_[i][read_i];

      if (pos < pos_read || pos > pos_read + seq.size() - 1) {
        continue;
      }

      if (const char base = seq[pos - pos_read]; base != '0') {
        seqs.push_back(seq[pos - pos_read]);
        quals.push_back(qual[pos - pos_read]);
      }
    }
  }
  return std::make_pair(seqs, quals);
}

// -----------------------------------------------------------------------------

uint64_t RecordPileup::GetMinPos() const { return min_pos_; }

// -----------------------------------------------------------------------------

uint64_t RecordPileup::GetMaxPos() const { return max_pos_; }

// -----------------------------------------------------------------------------

void RecordPileup::NextRecord() {
  preprocessed_qvalues_.emplace_back();
  preprocessed_sequences_.emplace_back();
}

// -----------------------------------------------------------------------------

std::vector<EncodingRecord> RecordPileup::GetRecordsBefore(const uint64_t pos) {
  // new vectors
  std::vector<std::vector<std::string>> new_pre_seqs;
  std::vector<std::vector<std::string>> new_pre_quals;
  std::vector<EncodingRecord> return_records;
  std::vector<EncodingRecord> new_records;

  if (pos <= this->min_pos_) {
    // return empty vectors
    return return_records;
  }

  if (pos > this->max_pos_) {
    // return all
    this->min_pos_ = static_cast<uint64_t>(-1);
    this->max_pos_ = 0;

    this->preprocessed_qvalues_.clear();
    this->preprocessed_sequences_.clear();
    std::swap(this->records_, return_records);

    return return_records;
  }

  for (uint64_t record_i = 0; record_i < this->records_.size(); ++record_i) {
    auto& record_positions = this->records_[record_i].positions;

    // move record to according vector
    if (auto& record_seqs = this->preprocessed_sequences_[record_i];
        IsRecordBeforePos(record_positions, record_seqs, pos)) {
      return_records.emplace_back(std::move(this->records_[record_i]));

    } else {
      new_records.emplace_back(std::move(this->records_[record_i]));
      new_pre_quals.emplace_back(
          std::move(this->preprocessed_qvalues_[record_i]));
      new_pre_seqs.emplace_back(
          std::move(this->preprocessed_sequences_[record_i]));
    }
  }

  // assign new vectors to class
  this->records_ = std::move(new_records);
  this->preprocessed_sequences_ = std::move(new_pre_seqs);
  this->preprocessed_qvalues_ = std::move(new_pre_quals);

  // assign new min/max values
  this->min_pos_ = this->records_.front().positions.front();

  return return_records;
}

// -----------------------------------------------------------------------------

std::vector<EncodingRecord> RecordPileup::GetAllRecords() {
  return this->records_;
}

// -----------------------------------------------------------------------------

bool RecordPileup::IsRecordBeforePos(
    const std::vector<uint64_t>& positions,
    const std::vector<std::string>& preprocessed_seqs, const uint64_t pos) {
  for (uint64_t read_i = 0; read_i < positions.size(); ++read_i) {
    if (const uint64_t read_max_pos =
            positions[read_i] + preprocessed_seqs[read_i].size() - 1;
        read_max_pos >= pos) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------

bool RecordPileup::empty() const { return records_.empty(); }

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
