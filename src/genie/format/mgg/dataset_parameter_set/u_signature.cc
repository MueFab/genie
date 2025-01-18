/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_parameter_set/u_signature.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::dataset_parameter_set {

// -----------------------------------------------------------------------------

bool USignature::operator==(const USignature& other) const {
  return u_signature_length == other.u_signature_length;
}

// -----------------------------------------------------------------------------

USignature::USignature() : u_signature_length(std::nullopt) {}

// -----------------------------------------------------------------------------

USignature::USignature(util::BitReader& reader) {
  if (reader.Read<bool>(1)) {
    u_signature_length = reader.Read<uint8_t>();
  }
}

// -----------------------------------------------------------------------------

bool USignature::IsConstantLength() const {
  return u_signature_length != std::nullopt;
}

// -----------------------------------------------------------------------------

uint8_t USignature::GetConstLength() const { return *u_signature_length; }

// -----------------------------------------------------------------------------

void USignature::SetConstLength(uint8_t length) { u_signature_length = length; }

// -----------------------------------------------------------------------------

void USignature::Write(util::BitWriter& writer) const {
  writer.WriteBits(u_signature_length != std::nullopt, 1);
  if (u_signature_length != std::nullopt) {
    writer.WriteBits(*u_signature_length, 8);
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_parameter_set

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
