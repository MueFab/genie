#include "access_unit_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

void MITAccessUnitInfo::write(util::BitWriter& bit_writer) const {

    // AU_byte_offset u(byteOffsetSize)
    bit_writer.write(AU_byte_offset, (uint8_t) byte_offset_size_flag);

    // AU_start_position u(posSize)
    bit_writer.write(AU_start_position, (uint8_t) pos_40_size_flag);

    // AU_end_position u(posSize)
    bit_writer.write(AU_end_position, (uint8_t) pos_40_size_flag);

    // ref_*
    if (ref_seq_info != nullptr){
        ref_seq_info->write(bit_writer);
    }

    // extended_AU_*
    if (extended_au_info != nullptr) {
        extended_au_info->write(bit_writer);
    }

    // block_byte_offset u(byteOffsetSize)
    for (auto offset: block_byte_offset){
        bit_writer.write(offset, (uint8_t) byte_offset_size_flag);
    }

}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie