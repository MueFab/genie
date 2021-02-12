#include "ref_seq_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {


MITReferenceSequenceInfo::MITReferenceSequenceInfo(util::BitReader& reader) {

    /// ref_seq_id u(16)
    ref_sequence_id = reader.read<uint16_t>();

    /// ref_start_position[seq][ci]][au_id] u(posSize)
    /// ref_end_position[seq][ci]][au_id] u(posSize)
    ref_start_position = reader.read<uint64_t>((uint8_t) pos_40_flag);
    ref_end_position = reader.read<uint64_t>((uint8_t) pos_40_flag);


}

MITReferenceSequenceInfo::MITReferenceSequenceInfo(uint16_t seq_id, uint64_t start_pos, uint64_t end_pos,
                                                   DatasetHeader::Pos40SizeFlag flag)
    : ref_sequence_id(seq_id),
      ref_start_position(start_pos),
      ref_end_position(end_pos),
      pos_40_flag(flag){}

uint64_t MITReferenceSequenceInfo::getBitLength() const {

    uint64_t bitlen;

    bitlen += 16;      /// ref_sequence_id u(16)

    /// ref_start_position[seq][ci]][au_id] u(posSize)
    /// ref_end_position[seq][ci]][au_id] u(posSize)
    bitlen += (uint64_t) pos_40_flag;
    bitlen += (uint64_t) pos_40_flag;

    return bitlen;
}

void MITReferenceSequenceInfo::write(util::BitWriter& bit_writer) const {

    // ref_sequence_id u(16)
    bit_writer.write(ref_sequence_id, 16);

    // ref_start_position u(posSize)
    bit_writer.write(ref_start_position, (uint8_t) pos_40_flag);

    // ref_end_position u(posSize)
    bit_writer.write(ref_end_position, (uint8_t) pos_40_flag);

}


}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie