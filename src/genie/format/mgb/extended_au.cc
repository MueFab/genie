/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgb/extended_au.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

bool ExtendedAu::operator==(const ExtendedAu& other) const {
    return extended_AU_start_position == other.extended_AU_start_position &&
           extended_AU_end_position == other.extended_AU_end_position && posSize == other.posSize;
}

// ---------------------------------------------------------------------------------------------------------------------

void ExtendedAu::write(util::BitWriter& writer) const {
    writer.write(extended_AU_start_position, posSize);
    writer.write(extended_AU_end_position, posSize);
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

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
