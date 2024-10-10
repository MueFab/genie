/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/extended_au.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgb {

// ---------------------------------------------------------------------------------------------------------------------

bool ExtendedAu::operator==(const ExtendedAu& other) const {
    return extended_AU_start_position == other.extended_AU_start_position &&
           extended_AU_end_position == other.extended_AU_end_position && posSize == other.posSize;
}

// ---------------------------------------------------------------------------------------------------------------------

void ExtendedAu::write(util::BitWriter& writer) const {
    writer.writeBits(extended_AU_start_position, posSize);
    writer.writeBits(extended_AU_end_position, posSize);
}

// ---------------------------------------------------------------------------------------------------------------------

ExtendedAu::ExtendedAu(uint64_t _extended_AU_start_position, uint64_t _extended_AU_end_position, uint8_t _posSize)
    : extended_AU_start_position(_extended_AU_start_position),
      extended_AU_end_position(_extended_AU_end_position),
      posSize(_posSize) {}

// ---------------------------------------------------------------------------------------------------------------------

ExtendedAu::ExtendedAu(uint8_t _posSize, util::BitReader& reader) : posSize(_posSize) {
    extended_AU_start_position = reader.read<uint64_t>(posSize);
    extended_AU_end_position = reader.read<uint64_t>(posSize);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgb

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
