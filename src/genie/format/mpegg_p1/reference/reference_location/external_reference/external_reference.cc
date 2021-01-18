#include "external_reference.h"


namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

//ExternalReference::ExternalReference(util::BitReader& reader):
//{
//
//}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReference::ExternalReference() {}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReference::ExternalReference(Type _reference_type):
    reference_type(_reference_type) {}

// ---------------------------------------------------------------------------------------------------------------------

ExternalReference::Type ExternalReference::getReferenceType() const { return reference_type; }

// ---------------------------------------------------------------------------------------------------------------------

//void ExternalReference::write(util::BitWriter& writer) const {
//    UTILS_DIE("Base class");
//}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie