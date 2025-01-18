/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/reference/sequence.h"

#include <string>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::format::mgg::reference {

// -----------------------------------------------------------------------------

bool Sequence::operator==(const Sequence& other) const {
  return name_ == other.name_ && sequence_length_ == other.sequence_length_ &&
         sequence_id_ == other.sequence_id_ && version_ == other.version_;
}

// -----------------------------------------------------------------------------

Sequence::Sequence(std::string name, const uint32_t length, const uint16_t id,
                   const core::MpegMinorVersion version)
    : name_(std::move(name)),
      sequence_length_(length),
      sequence_id_(id),
      version_(version) {}

// -----------------------------------------------------------------------------

const std::string& Sequence::GetName() const { return name_; }

// -----------------------------------------------------------------------------

uint32_t Sequence::GetLength() const { return sequence_length_; }

// -----------------------------------------------------------------------------

uint16_t Sequence::GetId() const { return sequence_id_; }

// -----------------------------------------------------------------------------

Sequence::Sequence(util::BitReader& reader,
                   const core::MpegMinorVersion version)
    : version_(version) {
  name_ = reader.ReadAlignedStringTerminated();
  if (version_ != core::MpegMinorVersion::kV1900) {
    sequence_length_ = reader.ReadAlignedInt<uint32_t>();
    sequence_id_ = reader.ReadAlignedInt<uint16_t>();
  }
}

// -----------------------------------------------------------------------------

void Sequence::Write(util::BitWriter& writer) const {
  writer.WriteAlignedBytes(name_.data(), name_.length());
  writer.WriteAlignedInt<uint8_t>('\0');
  if (version_ != core::MpegMinorVersion::kV1900) {
    writer.WriteAlignedInt(sequence_length_);
    writer.WriteAlignedInt(sequence_id_);
  }
}

// -----------------------------------------------------------------------------

Sequence::Sequence(core::meta::Sequence s,
                   const core::MpegMinorVersion version)
    : name_(std::move(s.GetName())),
      sequence_length_(static_cast<uint32_t>(s.GetLength())),
      sequence_id_(s.GetId()),
      version_(version) {}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
