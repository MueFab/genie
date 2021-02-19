#include "without_header.h"

namespace genie {
namespace format{
namespace mpegg_p1 {

WithoutHeader::WithoutHeader()
    : BlockConfig(false, true),
      ordered_blocks_flag(false){}

WithoutHeader::WithoutHeader(bool _cc_mode_flag)
    : BlockConfig(false, true),
      ordered_blocks_flag(_cc_mode_flag){}

void WithoutHeader::ReadWithoutHeader(util::BitReader& reader, size_t length) {

    size_t start_pos = reader.getPos();

    auto block_length = reader.read<size_t>();
    BlockConfig::ReadBlockConfig(reader, block_length);

    // ordered_blocks_flag u(1)
    ordered_blocks_flag = reader.read<bool>(1);

    UTILS_DIE_IF(reader.getPos() - start_pos != length, "Invalid ReadWithoutHeader length!");
}


uint64_t WithoutHeader::getBitLength() const {
    return BlockConfig::getBitLength() + 1; // block_header_flag, ordered_blocks_flag u(1)
}

void WithoutHeader::write(util::BitWriter& bit_writer) const {

    // block_header_flag u(1)
    BlockConfig::write(bit_writer);

    // ordered_blocks_flag u(1)
    bit_writer.write(ordered_blocks_flag, 1);
}


}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
