#include "genie/util/exception.h"
#include "external_reference.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

ExternalReference::ExternalReference(Type _reference_type):
    reference_type(_reference_type) {}

//void ExternalReference::writeToFile(util::BitWriter& bit_writer) const {
//    UTILS_DIE("Base class");
//}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie