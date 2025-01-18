/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/extended_au.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

// -----------------------------------------------------------------------------

bool ExtendedAu::operator==(const ExtendedAu& other) const {
  return extended_AU_start_position == other.extended_AU_start_position &&
         extended_AU_end_position == other.extended_AU_end_position &&
         posSize == other.posSize;
}

// -----------------------------------------------------------------------------

void ExtendedAu::Write(util::BitWriter& writer) const {
  writer.WriteBits(extended_AU_start_position, posSize);
  writer.WriteBits(extended_AU_end_position, posSize);
}

// -----------------------------------------------------------------------------

ExtendedAu::ExtendedAu(const uint64_t extended_au_start_position,
                       const uint64_t extended_au_end_position,
                       const uint8_t pos_size)
    : extended_AU_start_position(extended_au_start_position),
      extended_AU_end_position(extended_au_end_position),
      posSize(pos_size) {}

// -----------------------------------------------------------------------------

ExtendedAu::ExtendedAu(const uint8_t pos_size, util::BitReader& reader)
    : posSize(pos_size) {
  extended_AU_start_position = reader.Read<uint64_t>(posSize);
  extended_AU_end_position = reader.Read<uint64_t>(posSize);
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
