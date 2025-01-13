/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/alignment_split/same_rec.h"

#include <memory>
#include <utility>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core::record::alignment_split {

// -----------------------------------------------------------------------------

SameRec::SameRec() : AlignmentSplit(Type::kSameRec), delta_(0) {}

// -----------------------------------------------------------------------------

SameRec::SameRec(const int64_t delta, Alignment alignment)
    : AlignmentSplit(Type::kSameRec),
      delta_(delta),
      alignment_(std::move(alignment)) {}

// -----------------------------------------------------------------------------

SameRec::SameRec(const uint8_t as_depth, util::BitReader& reader)
    : AlignmentSplit(Type::kSameRec),
      delta_(reader.ReadAlignedInt<int64_t, 6>()),
      alignment_(as_depth, reader) {}

// -----------------------------------------------------------------------------

void SameRec::Write(util::BitWriter& writer) const {
  AlignmentSplit::Write(writer);
  writer.WriteAlignedInt<int64_t, 6>(delta_);
  alignment_.Write(writer);
}

// -----------------------------------------------------------------------------

const Alignment& SameRec::GetAlignment() const { return alignment_; }

// -----------------------------------------------------------------------------

int64_t SameRec::GetDelta() const { return delta_; }

// -----------------------------------------------------------------------------

std::unique_ptr<AlignmentSplit> SameRec::clone() const {
  auto ret = std::make_unique<SameRec>();
  ret->delta_ = this->delta_;
  ret->alignment_ = this->alignment_;
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_split

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
