
//#include <utility>
#include "checksum.h"
#include "mpegg_ref.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

MpegReference::MpegReference(uint8_t _dataset_group_ID, uint16_t _dataset_ID, Checksum &&_ref_checksum)
    : ExternalReference(ExternalReference::Type::MPEGG_REF),
      dataset_group_ID(_dataset_group_ID),
      dataset_ID(_dataset_ID),
      ref_checksum(_ref_checksum){}

void MpegReference::addChecksum(Checksum&& _checksum) {
    ref_checksum = _checksum;
}
void MpegReference::write(util::BitWriter& bit_writer) const {
    bit_writer.write(dataset_group_ID, 8);
    bit_writer.write(dataset_ID, 16);
    ref_checksum.write(bit_writer);
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie