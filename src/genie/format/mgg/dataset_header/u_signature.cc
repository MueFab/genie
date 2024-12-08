/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_header/u_signature.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

// -----------------------------------------------------------------------------
bool USignature::operator==(const USignature& other) const {
  return const_length_ == other.const_length_;
}

// -----------------------------------------------------------------------------
USignature::USignature() : const_length_(std::nullopt) {}

// -----------------------------------------------------------------------------
USignature::USignature(uint8_t const_length) : const_length_(const_length) {}

// -----------------------------------------------------------------------------
USignature::USignature(util::BitReader& reader) {
  if (reader.Read<bool>(1)) {
    const_length_ = reader.Read<uint8_t>(8);
  }
}

// -----------------------------------------------------------------------------
void USignature::Write(util::BitWriter& writer) const {
  writer.WriteBits(IsConstLength(), 1);
  if (IsConstLength()) {
    writer.WriteBits(GetConstLength(), 8);
  }
}

// -----------------------------------------------------------------------------
bool USignature::IsConstLength() const { return const_length_ != std::nullopt; }

// -----------------------------------------------------------------------------
uint8_t USignature::GetConstLength() const { return *const_length_; }

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
