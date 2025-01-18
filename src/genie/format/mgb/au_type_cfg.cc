/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/au_type_cfg.h"

#include <utility>

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

// -----------------------------------------------------------------------------

bool AuTypeCfg::operator==(const AuTypeCfg& other) const {
  return sequence_ID == other.sequence_ID &&
         AU_start_position == other.AU_start_position &&
         AU_end_position == other.AU_end_position &&
         extended_AU == other.extended_AU && posSize == other.posSize;
}

// -----------------------------------------------------------------------------

void AuTypeCfg::Write(util::BitWriter& writer) const {
  writer.WriteBits(sequence_ID, 16);
  writer.WriteBits(AU_start_position, posSize);
  writer.WriteBits(AU_end_position, posSize);
  if (extended_AU) {
    extended_AU->Write(writer);
  }
}

// -----------------------------------------------------------------------------

AuTypeCfg::AuTypeCfg(const uint16_t sequence_id,
                     const uint64_t au_start_position,
                     const uint64_t au_end_position, const uint8_t pos_size)
    : sequence_ID(sequence_id),
      AU_start_position(au_start_position),
      AU_end_position(au_end_position),
      posSize(pos_size) {}

// -----------------------------------------------------------------------------

AuTypeCfg::AuTypeCfg(const uint8_t pos_size) : AuTypeCfg(0, 0, 0, pos_size) {}

// -----------------------------------------------------------------------------

AuTypeCfg::AuTypeCfg(const uint8_t pos_size, const bool multiple_alignments,
                     util::BitReader& reader)
    : posSize(pos_size) {
  sequence_ID = reader.Read<uint16_t>();
  AU_start_position = reader.Read<uint64_t>(posSize);
  AU_end_position = reader.Read<uint64_t>(posSize);
  if (multiple_alignments) {
    extended_AU = ExtendedAu(pos_size, reader);
  }
}

// -----------------------------------------------------------------------------

void AuTypeCfg::SetExtendedAu(ExtendedAu&& extended_au) {
  extended_AU = std::move(extended_au);
}

// -----------------------------------------------------------------------------

uint16_t AuTypeCfg::GetRefId() const { return sequence_ID; }

// -----------------------------------------------------------------------------

uint64_t AuTypeCfg::GetStartPos() const { return AU_start_position; }

// -----------------------------------------------------------------------------

uint64_t AuTypeCfg::GetEndPos() const { return AU_end_position; }

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
