/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/alignment_split/other_rec.h"

#include <memory>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core::record::alignment_split {

// -----------------------------------------------------------------------------

OtherRec::OtherRec(const uint64_t split_pos, const uint16_t split_seq_id,
                   const std::optional<Alignment>& extended_alignment)
    : AlignmentSplit(Type::kOtherRec),
      split_pos_(split_pos),
      split_seq_id_(split_seq_id),
      extended_alignment_(extended_alignment) {}

// -----------------------------------------------------------------------------

OtherRec::OtherRec(const bool extended_alignment, util::BitReader& reader)
    : AlignmentSplit(Type::kOtherRec),
      split_pos_(reader.ReadAlignedInt<uint64_t, 5>()),
      split_seq_id_(reader.ReadAlignedInt<uint16_t>()) {
  if (extended_alignment) {
    extended_alignment_ = Alignment(1, false, reader);
  }
}

// -----------------------------------------------------------------------------

OtherRec::OtherRec()
    : AlignmentSplit(Type::kOtherRec), split_pos_(0), split_seq_id_(0) {}

// -----------------------------------------------------------------------------

uint64_t OtherRec::GetNextPos() const { return split_pos_; }

// -----------------------------------------------------------------------------

uint16_t OtherRec::GetNextSeq() const { return split_seq_id_; }

// -----------------------------------------------------------------------------

std::optional<Alignment> OtherRec::GetExtendedAlignment() const {
  return extended_alignment_;
}

// -----------------------------------------------------------------------------

void OtherRec::Write(util::BitWriter& writer) const {
  AlignmentSplit::Write(writer);
  writer.WriteAlignedInt<uint64_t, 5>(split_pos_);
  writer.WriteAlignedInt(split_seq_id_);
  if (extended_alignment_) {
    extended_alignment_->Write(writer);
  }
}

// -----------------------------------------------------------------------------

std::unique_ptr<AlignmentSplit> OtherRec::clone() const {
  auto ret = std::make_unique<OtherRec>();
  ret->split_pos_ = this->split_pos_;
  ret->split_seq_id_ = this->split_seq_id_;
  ret->extended_alignment_ = this->extended_alignment_;
  return ret;
}

// -----------------------------------------------------------------------------

[[nodiscard]] std::optional<bool> OtherRec::IsExtendedAlignment() const {
  return extended_alignment_.has_value();
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_split

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
