/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/reference/reference_location/external_reference/external_reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

ExternalReference::ExternalReference(): reference_type(Type::UNKNOWN) {}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReference::ExternalReference(Type _reference_type) : reference_type(_reference_type) {}

// ---------------------------------------------------------------------------------------------------------------------

Checksum::Algo ExternalReference::getChecksumAlg() const {
    UTILS_DIE("This is base class");
}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReference::Type ExternalReference::getReferenceType() const { return reference_type; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ExternalReference::getLength() const {
    UTILS_DIE("This is base class");
}

// ---------------------------------------------------------------------------------------------------------------------

void ExternalReference::write(genie::util::BitWriter& bitwriter) const {
    bitwriter.write((uint64_t) reference_type, 8);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
