/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/reference/reference_location/reference_location.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

ReferenceLocation::ReferenceLocation() : external_ref_flag(ReferenceLocation::Flag::INTERNAL) {}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceLocation::ReferenceLocation(ReferenceLocation::Flag _flag) : external_ref_flag(_flag) {}

// ---------------------------------------------------------------------------------------------------------------------

bool ReferenceLocation::isExternal() const { return external_ref_flag == Flag::EXTERNAL; }

// ---------------------------------------------------------------------------------------------------------------------

ReferenceLocation::Flag ReferenceLocation::getExternalRefFlag() const { return external_ref_flag; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ReferenceLocation::getLength() const { return 1; }

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceLocation::write(util::BitWriter& bitwriter) const {
    bitwriter.write(0, 7);
    bitwriter.write((uint64_t)external_ref_flag, 1);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
