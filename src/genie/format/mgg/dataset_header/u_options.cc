/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_header/u_options.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

// -----------------------------------------------------------------------------

bool UOptions::operator==(const UOptions& other) const {
  return reserved1_ == other.reserved1_ && u_signature_ == other.u_signature_ &&
         reserved2_ == other.reserved2_ && reserved3_ == other.reserved3_;
}

// -----------------------------------------------------------------------------

UOptions::UOptions(const uint64_t reserved1, const bool reserved3)
    : reserved1_(reserved1),
      u_signature_(std::nullopt),
      reserved2_(std::nullopt),
      reserved3_(reserved3) {}

// -----------------------------------------------------------------------------

UOptions::UOptions(util::BitReader& reader) : reserved3_(false) {
  reserved1_ = reader.Read<uint64_t>(62);
  if (reader.Read<bool>(1)) {
    u_signature_ = USignature(reader);
  }
  if (reader.Read<bool>(1)) {
    reserved2_ = reader.Read<uint8_t>(8);
  }
  //  reserved3 = reader.read<bool>(1); // TODO(muenteferi): Reference decoder
  //  and reference bitstreams contradict document
}

// -----------------------------------------------------------------------------

void UOptions::Write(util::BitWriter& writer) const {
  writer.WriteBits(reserved1_, 62);
  writer.WriteBits(HasSignature(), 1);
  if (HasSignature()) {
    u_signature_->Write(writer);
  }
  writer.WriteBits(HasReserved2(), 1);
  if (HasReserved2()) {
    writer.WriteBits(GetReserved2(), 8);
  }
  writer.WriteBits(reserved3_, 1);
}

// -----------------------------------------------------------------------------

uint64_t UOptions::GetReserved1() const { return reserved1_; }

// -----------------------------------------------------------------------------

bool UOptions::GetReserved3() const { return reserved3_; }

// -----------------------------------------------------------------------------

bool UOptions::HasReserved2() const { return reserved2_ != std::nullopt; }

// -----------------------------------------------------------------------------

uint8_t UOptions::GetReserved2() const { return *reserved2_; }

// -----------------------------------------------------------------------------

bool UOptions::HasSignature() const { return u_signature_ != std::nullopt; }

// -----------------------------------------------------------------------------

const USignature& UOptions::GetSignature() const { return *u_signature_; }

// -----------------------------------------------------------------------------

void UOptions::AddSignature(USignature s) { u_signature_ = s; }

// -----------------------------------------------------------------------------

void UOptions::AddReserved2(uint8_t r) { reserved2_ = r; }

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
