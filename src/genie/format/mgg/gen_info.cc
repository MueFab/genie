/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/gen_info.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

bool GenInfo::operator==(const GenInfo& info) const { return getKey() == info.getKey(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t GenInfo::getHeaderLength() { return sizeof(uint64_t) + sizeof(char) * 4; }

// ---------------------------------------------------------------------------------------------------------------------

void GenInfo::write(genie::util::BitWriter& bitWriter) const {
    int64_t begin = bitWriter.getStreamPosition();
    bitWriter.writeAlignedBytes(getKey().data(), getKey().length());
    int64_t size_pos = bitWriter.getStreamPosition();
    bitWriter.writeAlignedInt<uint64_t>(0);
    box_write(bitWriter);
    int64_t end = bitWriter.getStreamPosition();
    bitWriter.setStreamPosition(size_pos);
    bitWriter.writeAlignedInt<uint64_t>(end - begin);
    bitWriter.setStreamPosition(end);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
