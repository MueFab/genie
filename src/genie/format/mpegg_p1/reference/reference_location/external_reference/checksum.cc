/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/reference/reference_location/external_reference/checksum.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Checksum::Checksum(): checksum_alg(Algo::UNKNOWN) {}

// ---------------------------------------------------------------------------------------------------------------------

Checksum::Checksum(Algo _algo) : checksum_alg(_algo) {}

// ---------------------------------------------------------------------------------------------------------------------

//Checksum::Checksum(Checksum&& _container) noexcept { *this = _container; }

// ---------------------------------------------------------------------------------------------------------------------

Checksum::Algo Checksum::getType() const { return checksum_alg; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Checksum::getLength() const {
    UTILS_DIE("This is the base class of Checksum");
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

void Checksum::write(util::BitWriter& bit_writer) const {
    UTILS_DIE("This is the base class of Checksum");
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
