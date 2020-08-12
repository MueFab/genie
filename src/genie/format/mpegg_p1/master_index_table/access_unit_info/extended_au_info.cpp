
#include "extended_au_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

MITExtendedAUInfo::MITExtendedAUInfo(uint64_t start_pos, uint64_t end_pos, DatasetHeader::Pos40SizeFlag flag)
    : extend_AU_start_position(start_pos),
      extend_AU_end_position(end_pos),
      pos_40_flag(flag){}

void MITExtendedAUInfo::write(util::BitWriter& bit_writer) const {

    // extended_AU_start_position u(posSize)
    bit_writer.write(extend_AU_start_position, (uint8_t) pos_40_flag);

    // extended_AU_start_position u(posSize)
    bit_writer.write(extend_AU_end_position, (uint8_t) pos_40_flag);

}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie