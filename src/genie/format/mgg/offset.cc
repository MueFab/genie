/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/offset.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

// ---------------------------------------------------------------------------------------------------------------------

Offset::Offset(util::BitReader& bitReader) : subkey(4, '\0') {
    bitReader.readBypass(subkey);
    offset = bitReader.readBypassBE<uint64_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

void Offset::write(util::BitWriter& writer) const {
    writer.writeBypass("offs", 4);
    writer.writeBypass(subkey.data(), 4);
    writer.writeBypassBE(offset);
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Offset::getSubkey() const { return subkey; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Offset::getOffset() const { return offset; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
