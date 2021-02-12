
#include "extended_au_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {


MITExtendedAUInfo::MITExtendedAUInfo(util::BitReader& reader) {

    /// extended_AU_start_position[seq][ci]][au_id] u(posSize)
    /// extended_AU_end_position[seq][ci]][au_id] u(posSize)
    extended_AU_start_position = reader.read<uint64_t>((uint8_t) pos_40_flag);
    extended_AU_end_position = reader.read<uint64_t>((uint8_t) pos_40_flag);
}

MITExtendedAUInfo::MITExtendedAUInfo(uint64_t start_pos, uint64_t end_pos, DatasetHeader::Pos40SizeFlag flag)
    : extended_AU_start_position(start_pos),
      extended_AU_end_position(end_pos),
      pos_40_flag(flag){}


uint64_t MITExtendedAUInfo::getBitLength() const {

    uint64_t bitlen;

    /// extended_AU_start_position[seq][ci]][au_id] u(posSize)
    /// extended_AU_end_position[seq][ci]][au_id] u(posSize)
    bitlen += (uint64_t) pos_40_flag;
    bitlen += (uint64_t) pos_40_flag;

    return bitlen;
}

void MITExtendedAUInfo::write(util::BitWriter& bit_writer) const {

    // extended_AU_start_position u(posSize)
    bit_writer.write(extended_AU_start_position, (uint8_t) pos_40_flag);

    // extended_AU_start_position u(posSize)
    bit_writer.write(extended_AU_end_position, (uint8_t) pos_40_flag);

}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie