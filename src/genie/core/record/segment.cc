/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/segment.h"

#include <string>
#include <utility>
#include <vector>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::core::record {

// -----------------------------------------------------------------------------
Segment::Segment() = default;

// -----------------------------------------------------------------------------
Segment::Segment(std::string&& sequence) : sequence_(std::move(sequence)) {}

// -----------------------------------------------------------------------------
Segment::Segment(const uint32_t length, const uint8_t qv_depth,
                 util::BitReader& reader)
    : sequence_(length, 0), quality_values_(qv_depth, std::string(length, 0)) {
  reader.ReadAlignedBytes(&this->sequence_[0], length);
  for (auto& q : quality_values_) {
    reader.ReadAlignedBytes(&q[0], q.length());
  }
}

// -----------------------------------------------------------------------------
const std::string& Segment::GetSequence() const { return sequence_; }

// -----------------------------------------------------------------------------
const std::vector<std::string>& Segment::GetQualities() const {
  return quality_values_;
}

// -----------------------------------------------------------------------------
void Segment::AddQualities(std::string&& qv) {
  // Source:
  //  From Section 10.2.16
  // quality_values[rSeq][qs] array shall be a c(read_len[rSeg]), otherwise
  // quality_values[rSeq][qs] shall be a c(1) string containing only one
  // character corresponding to ASCII code 0
  static const std::string no_quality("\0", 1);
  UTILS_DIE_IF(!(qv == no_quality || qv.length() == sequence_.length()),
               "QV and sequence lengths do not match");
  quality_values_.emplace_back(std::move(qv));
}

// -----------------------------------------------------------------------------
void Segment::Write(util::BitWriter& writer) const {
  writer.WriteAlignedBytes(this->sequence_.data(), this->sequence_.length());
  for (const auto& a : this->quality_values_) {
    writer.WriteAlignedBytes(a.data(), a.length());
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
