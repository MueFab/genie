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

OtherRec::OtherRec(const uint64_t split_pos, const uint16_t split_seq_id)
    : AlignmentSplit(Type::kOtherRec),
      split_pos_(split_pos),
      split_seq_id_(split_seq_id) {}

// -----------------------------------------------------------------------------

OtherRec::OtherRec(util::BitReader& reader)
    : AlignmentSplit(Type::kOtherRec),
      split_pos_(reader.ReadAlignedInt<uint64_t, 5>()),
      split_seq_id_(reader.ReadAlignedInt<uint16_t>()) {}

// -----------------------------------------------------------------------------

OtherRec::OtherRec()
    : AlignmentSplit(Type::kOtherRec), split_pos_(0), split_seq_id_(0) {}

// -----------------------------------------------------------------------------

uint64_t OtherRec::GetNextPos() const { return split_pos_; }

// -----------------------------------------------------------------------------

uint16_t OtherRec::GetNextSeq() const { return split_seq_id_; }

// -----------------------------------------------------------------------------

void OtherRec::Write(util::BitWriter &writer) const {
    AlignmentSplit::Write(writer);
    writer.WriteBypassBE<uint64_t, 5>(split_pos_);
    writer.WriteBypassBE(split_seq_id_);
}

// -----------------------------------------------------------------------------

std::unique_ptr<AlignmentSplit> OtherRec::clone() const {
  auto ret = std::make_unique<OtherRec>();
  ret->split_pos_ = this->split_pos_;
  ret->split_seq_id_ = this->split_seq_id_;
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_split

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
