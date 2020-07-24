//#include "genie/util/exceptions.h"
#include "external.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

External::External(std::string&& _ref_uri, ExternalReference&& _ext_ref)
    : ReferenceLocation(ReferenceLocation::Flag::EXTERNAL),
      ref_uri(_ref_uri),
      external_reference(_ext_ref){}

void External::write(util::BitWriter& bit_writer) const {
    // ref_uri st(v)
    bit_writer.write(ref_uri);
    // the rest
    external_reference.write(bit_writer);
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
