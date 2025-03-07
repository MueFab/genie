/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "other_rec.h"

#include <memory>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::alignment_external {

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec(const uint64_t _next_pos, const uint16_t next_seq_id)
    : AlignmentExternal(AlignmentExternal::Type::kOtherRec),
      next_pos_(_next_pos),
      next_seq_id_(next_seq_id) {}

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec(util::BitReader &reader)
    : AlignmentExternal(AlignmentExternal::Type::kOtherRec),
      next_pos_(reader.ReadAlignedInt<uint64_t, 5>()),
      next_seq_id_(reader.ReadAlignedInt<uint16_t>()) {}

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec()
    : AlignmentExternal(AlignmentExternal::Type::kOtherRec), next_pos_(0), next_seq_id_(0) {}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t OtherRec::GetNextPos() const { return next_pos_; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t OtherRec::GetNextSeq() const { return next_seq_id_; }

// ---------------------------------------------------------------------------------------------------------------------

void OtherRec::Write(util::BitWriter &writer) const {
  AlignmentExternal::Write(writer);
  writer.WriteBypassBE<uint64_t, 5>(next_pos_);
  writer.WriteBypassBE(next_seq_id_);
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<AlignmentExternal> OtherRec::Clone() const {
  auto ret = std::make_unique<OtherRec>();
  ret->next_pos_ = this->next_pos_;
  ret->next_seq_id_ = this->next_seq_id_;
  return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_external

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
