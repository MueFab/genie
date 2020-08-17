#include "without_header.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

WithoutHeader::WithoutHeader()
    : BlockConfig(false, true),
      ordered_blocks_flag(false){}

WithoutHeader::WithoutHeader(bool _cc_mode_flag)
    : BlockConfig(false, true),
      ordered_blocks_flag(_cc_mode_flag){}

uint64_t WithoutHeader::getLength() const {
    return BlockConfig::getLength() + 1; // block_header_flag, ordered_blocks_flag
}

void WithoutHeader::write(genie::util::BitWriter& bit_writer) const {

    // block_header_flag u(1)
    BlockConfig::write(bit_writer);

//    // block_header_flag u(1)
//    bit_writer.write(getBlockHeaderFlag(), 1);

    // ordered_blocks_flag u(1)
    bit_writer.write(ordered_blocks_flag, 1);
}


}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
