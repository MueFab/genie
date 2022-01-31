/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/gen_info.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

uint64_t GenInfo::getSize() const { return 4 + sizeof(uint64_t); }

// ---------------------------------------------------------------------------------------------------------------------

void GenInfo::write(genie::util::BitWriter& bitWriter) const {
    bitWriter.writeBypass(getKey().data(), getKey().length());
    bitWriter.writeBypassBE<uint64_t>(getSize());
}

// ---------------------------------------------------------------------------------------------------------------------

bool GenInfo::operator==(const GenInfo& info) const { return getKey() == info.getKey(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
