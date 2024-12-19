/**
 * Copyright 2018-2024 The Genie Authors.
 * @file local_reference.cc
 *
 * Implementation of the LocalReference class for local sequence assembly in the
 * Genie framework.
 *
 * This file contains the implementation of the `LocalReference` class within
 * the `localassembly` namespace. The class manages local references for
 * assembly operations, including the handling of sequencing reads, their
 * alignments, and the generation of reference sequences using majority voting.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/localassembly/local_reference.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>

#include "genie/core/constants.h"
#include "genie/core/record/alignment_split/same_rec.h"
#include "genie/util/log.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "LocalAssembly";

namespace genie::read::localassembly {

// -----------------------------------------------------------------------------
LocalReference::LocalReference(const uint32_t cr_buf_max_size)
    : cr_buf_max_size_(cr_buf_max_size), cr_buf_size_(0) {}

// -----------------------------------------------------------------------------
std::string LocalReference::preprocess(const std::string& read,
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
void LocalReference::AddSingleRead(const std::string& record,
                                   const std::string& ecigar,
                                   const uint64_t position) {
  sequence_positions_.push_back(position);
  const std::string read = preprocess(record, ecigar);
  sequences_.push_back(read);
  cr_buf_size_ += static_cast<uint32_t>(read.length());

  while (cr_buf_size_ > cr_buf_max_size_) {
    if (sequences_.size() == 1) {
      UTILS_THROW_RUNTIME_EXCEPTION(
          "Read too long for current cr_buf_max_size");
    }
    // Erase oldest read
    cr_buf_size_ -= static_cast<uint32_t>(sequences_.front().length());
    sequences_.erase(sequences_.begin());
    sequence_positions_.erase(sequence_positions_.begin());
  }
}

// -----------------------------------------------------------------------------
void LocalReference::AddRead(const core::record::Record& s) {
  const auto& seq1 = s.GetSegments()[0].GetSequence();
  const auto& cigar1 = s.GetAlignments().front().GetAlignment().GetECigar();
  const auto pos1 = s.GetAlignments().front().GetPosition();
  AddSingleRead(seq1, cigar1, pos1);

  if (s.GetSegments().size() == 1) {
    return;
  }

  UTILS_DIE_IF(
      s.GetAlignments().front().GetAlignmentSplits().front()->GetType() !=
          core::record::AlignmentSplit::Type::kSameRec,
      "Only same record split alignments supported");

  const auto ptr = s.GetAlignments().front().GetAlignmentSplits().front().get();
  const auto& rec =
      dynamic_cast<const core::record::alignment_split::SameRec&>(*ptr);

  const auto& seq2 = s.GetSegments()[1].GetSequence();
  const auto& cigar2 = rec.GetAlignment().GetECigar();
  const auto pos2 = s.GetAlignments().front().GetPosition() + rec.GetDelta();
  AddSingleRead(seq2, cigar2, pos2);
}

// -----------------------------------------------------------------------------
std::string LocalReference::GenerateRef(const uint32_t offset,
                                        const uint32_t len) const {
  std::string ref;
  for (uint32_t i = offset; i < offset + len; ++i) {
    ref += MajorityVote(i);
  }
  return ref;
}

// -----------------------------------------------------------------------------
uint32_t LocalReference::LengthFromCigar(const std::string& cigar) {
  uint32_t len = 0;
  uint32_t count = 0;
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
      len += 1;
      continue;
    }
    switch (cigar_pos) {
      case '=':
      case '-':
      case '*':
      case '/':
      case '%':
        len += count;
        break;

      case '+':
      case ')':
      case ']':
        break;
      default:
        UTILS_THROW_RUNTIME_EXCEPTION("Unknown CIGAR character");
    }
    count = 0;
  }
  return len;
}

// -----------------------------------------------------------------------------
std::string LocalReference::GetReference(const uint32_t pos_offset,
                                         const std::string& cigar) const {
  return GenerateRef(pos_offset, LengthFromCigar(cigar));
}

// -----------------------------------------------------------------------------
std::string LocalReference::GetReference(const uint32_t pos_offset,
                                         const uint32_t len) const {
  return GenerateRef(pos_offset, len);
}

// -----------------------------------------------------------------------------
char LocalReference::MajorityVote(const uint32_t offset_to_first) const {
  std::map<char, uint16_t> votes;

  // Collect all alignments
  for (size_t i = 0; i < sequences_.size(); ++i) {
    if (const int64_t distance = static_cast<int64_t>(offset_to_first) -
                                 static_cast<int64_t>(sequence_positions_[i]);
        distance >= 0 &&
        static_cast<uint64_t>(distance) < sequences_[i].length()) {
      if (char c = sequences_[i][distance]; c != '0') {
        votes[c]++;
      }
    }
  }

  // Find max
  char max = '\0';
  uint16_t max_value = 0;
  for (const auto& [fst, snd] : votes) {
    if (max_value < snd) {
      max_value = snd;
      max = fst;
    } else if (max_value == snd) {
      max = GetAlphabetProperties(core::AlphabetId::kAcgtn)
                .lut[std::min(GetAlphabetProperties(core::AlphabetId::kAcgtn)
                                  .inverse_lut[max],
                              GetAlphabetProperties(core::AlphabetId::kAcgtn)
                                  .inverse_lut[fst])];
    }
  }

  return max;
}

// -----------------------------------------------------------------------------
void LocalReference::PrintWindow() const {
  const uint64_t min_pos = GetWindowBorder();
  for (size_t i = 0; i < sequences_.size(); ++i) {
    const uint64_t total_offset = sequence_positions_[i] - min_pos;
    std::stringstream ss;
    for (size_t s = 0; s < total_offset; ++s) {
      ss << ".";
    }
    ss << sequences_[i];
    UTILS_LOG(util::Logger::Severity::INFO, ss.str());
  }
}

// -----------------------------------------------------------------------------
uint64_t LocalReference::GetWindowBorder() const {
  uint64_t min_pos = std::numeric_limits<uint64_t>::max();
  for (auto& p : sequence_positions_) {
    min_pos = std::min(min_pos, p);
  }
  return min_pos;
}

// -----------------------------------------------------------------------------
uint32_t LocalReference::GetMaxBufferSize() const { return cr_buf_max_size_; }

// -----------------------------------------------------------------------------

}  // namespace genie::read::localassembly

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
