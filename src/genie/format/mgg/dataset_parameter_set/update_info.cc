/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_parameter_set/update_info.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::dataset_parameter_set {

// -----------------------------------------------------------------------------
bool UpdateInfo::operator==(const UpdateInfo& other) const {
  return multiple_alignment_flag_ == other.multiple_alignment_flag_ &&
         pos_40_bits_flag_ == other.pos_40_bits_flag_ &&
         alphabet_id_ == other.alphabet_id_ &&
         u_signature_ == other.u_signature_;
}

// -----------------------------------------------------------------------------
UpdateInfo::UpdateInfo(const bool multiple_alignment_flag,
                       const bool pos_40_bits_flag,
                       const core::AlphabetId alphabet_id)
    : multiple_alignment_flag_(multiple_alignment_flag),
      pos_40_bits_flag_(pos_40_bits_flag),
      alphabet_id_(alphabet_id) {}

// -----------------------------------------------------------------------------
UpdateInfo::UpdateInfo(util::BitReader& reader) {
  multiple_alignment_flag_ = reader.Read<bool>(1);
  pos_40_bits_flag_ = reader.Read<bool>(1);
  alphabet_id_ = reader.Read<core::AlphabetId>(8);
  if (reader.Read<bool>(1)) {
    u_signature_ = USignature(reader);
  }
  reader.FlushHeldBits();
}

// -----------------------------------------------------------------------------
void UpdateInfo::Write(util::BitWriter& writer) const {
  writer.WriteBits(multiple_alignment_flag_, 1);
  writer.WriteBits(pos_40_bits_flag_, 1);
  writer.WriteBits(static_cast<uint8_t>(alphabet_id_), 8);
  writer.WriteBits(u_signature_ != std::nullopt, 1);
  if (u_signature_ != std::nullopt) {
    u_signature_->Write(writer);
  }
  writer.FlushBits();
}

// -----------------------------------------------------------------------------
void UpdateInfo::AddUSignature(USignature signature) {
  u_signature_ = signature;
}

// -----------------------------------------------------------------------------
bool UpdateInfo::GetMultipleAlignmentFlag() const {
  return multiple_alignment_flag_;
}

// -----------------------------------------------------------------------------
bool UpdateInfo::GetPos40BitsFlag() const { return pos_40_bits_flag_; }

// -----------------------------------------------------------------------------
core::AlphabetId UpdateInfo::GetAlphabetId() const { return alphabet_id_; }

// -----------------------------------------------------------------------------
bool UpdateInfo::HasUSignature() const { return u_signature_ != std::nullopt; }

// -----------------------------------------------------------------------------
const USignature& UpdateInfo::GetUSignature() const { return *u_signature_; }

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_parameter_set

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
