/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/alignment.h"

#include <string>
#include <utility>
#include <vector>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core::record {

// -----------------------------------------------------------------------------
Alignment::Alignment(std::string&& e_cigar_string, const uint8_t reverse_comp)
    : e_cigar_string_(std::move(e_cigar_string)), reverse_comp_(reverse_comp) {}

// -----------------------------------------------------------------------------
Alignment::Alignment(const uint8_t as_depth, util::BitReader& reader) {
  e_cigar_string_.resize(reader.ReadAlignedInt<uint32_t, 3>());
  reader.ReadAlignedBytes(&e_cigar_string_[0], e_cigar_string_.size());

  reverse_comp_ = reader.ReadAlignedInt<uint8_t>();
  mapping_score_.resize(as_depth);
  reader.ReadAlignedBytes(&mapping_score_[0], as_depth * sizeof(int32_t));
  for (auto& s : mapping_score_) {
    util::SwapEndianness(s);
  }
}

// -----------------------------------------------------------------------------
Alignment::Alignment() : reverse_comp_(0), mapping_score_(0) {}

// -----------------------------------------------------------------------------
void Alignment::AddMappingScore(const int32_t score) {
  mapping_score_.push_back(score);
}

// -----------------------------------------------------------------------------
const std::vector<int32_t>& Alignment::GetMappingScores() const {
  return mapping_score_;
}

// -----------------------------------------------------------------------------
const std::string& Alignment::GetECigar() const { return e_cigar_string_; }

// -----------------------------------------------------------------------------
uint8_t Alignment::GetRComp() const { return this->reverse_comp_; }

// -----------------------------------------------------------------------------
void Alignment::Write(util::BitWriter& writer) const {
  writer.WriteAlignedInt<uint32_t, 3>(
      static_cast<uint32_t>(e_cigar_string_.length()));
  writer.WriteAlignedBytes(e_cigar_string_.data(), e_cigar_string_.length());
  writer.WriteAlignedInt(reverse_comp_);
  for (const auto s : mapping_score_) {
    writer.WriteAlignedInt(s);
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
