#include "ref_seq_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {


MITReferenceSequenceInfo::MITReferenceSequenceInfo(uint16_t seq_id, uint64_t start_pos, uint64_t end_pos,
                                                   DatasetHeader::Pos40SizeFlag flag)
    : ref_sequence_id(seq_id),
      ref_start_position(start_pos),
      ref_end_position(end_pos),
      pos_40_flag(flag){}

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