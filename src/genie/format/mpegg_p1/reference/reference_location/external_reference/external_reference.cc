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

ExternalReference::ExternalReference() {}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReference::ExternalReference(Type _reference_type) : reference_type(_reference_type) {}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReference::Type ExternalReference::getReferenceType() const { return reference_type; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
