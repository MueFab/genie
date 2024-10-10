/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/offset.h"
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

Offset::Offset(util::BitReader& bitReader) : subkey(4, '\0') {
    bitReader.readAlignedBytes(subkey.data(), subkey.length());
    offset = bitReader.readAlignedInt<uint64_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

void Offset::write(util::BitWriter& writer) const {
    writer.writeAlignedBytes("offs", 4);
    writer.writeAlignedBytes(subkey.data(), 4);
    writer.writeAlignedInt(offset);
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Offset::getSubkey() const { return subkey; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Offset::getOffset() const { return offset; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
