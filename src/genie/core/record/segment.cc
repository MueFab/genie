/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/segment.h"

#include <string>
#include <utility>
#include <vector>

#include "genie/core/record/tag.h"
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
    reader.ReadAlignedBytes(&q[0], 1);
    if (q[0] == '\0') {
      q = std::string();
    } else {
      reader.ReadAlignedBytes(&q[1], q.length() - 1);
    }
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
  UTILS_DIE_IF(!(qv.empty() || qv.length() == sequence_.length()),
               "QV and sequence lengths do not match");
  quality_values_.emplace_back(std::move(qv));
}

// -----------------------------------------------------------------------------

void Segment::Write(util::BitWriter& writer) const {
  writer.WriteAlignedBytes(this->sequence_.data(), this->sequence_.length());
  for (const auto& a : this->quality_values_) {
    if (a.empty()) {
      writer.WriteAlignedBytes("\0", 1);
    } else {
      writer.WriteAlignedBytes(a.data(), a.length());
    }
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
