/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <genie/util/runtime-exception.h>
#include <genie/format/mpegg_p1/util.h>

#include "descriptor_stream_header.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

DescriptorStreamHeader::DescriptorStreamHeader()
    : reserved(),
      descriptor_ID(),
      class_ID(),
      num_blocks() {}


DescriptorStreamHeader::DescriptorStreamHeader(uint8_t _res, uint8_t _descriptor_ID, uint8_t _class_ID,
                                               uint32_t _num_blocks)
    : reserved(_res),
      descriptor_ID(_descriptor_ID),
      class_ID(_class_ID),
      num_blocks(_num_blocks) {}

DescriptorStreamHeader::DescriptorStreamHeader(util::BitReader& bit_reader) {

    std::string key = readKey(bit_reader);
    UTILS_DIE_IF(key != "dshd", "DescriptorStreamHeader is not Found");

    /// reserved u(1)
    reserved = bit_reader.read<uint8_t>(1);
    /// descriptor_ID u(7)
    descriptor_ID = bit_reader.read<uint8_t>(7);
    /// class_ID u(4)
    class_ID = bit_reader.read<uint8_t>(4);
    /// num_blocks u(32)
    num_blocks = bit_reader.read<uint32_t>();

    bit_reader.flush();
}


uint64_t DescriptorStreamHeader::getLength() const {

    /// length is first calculated in bits then converted in bytes

    /// Key c(4) Length u(64)
    uint64_t bitlen = (4 * sizeof(char) + 8) * 8;  // gen_info

    // reserved u(1)
    bitlen += 1;

    // descriptor_ID u(7)
    bitlen += 7;

    // class_ID u(4)
    bitlen += 4;

    // num_blocks u(32)
    bitlen += 32;

    /// !byte_aligned()
    bitlen += bitlen % 8;
    /// byte conversion
    bitlen /= 8;

    return bitlen;
}

void DescriptorStreamHeader::write(util::BitWriter& bit_writer) const {

    // Key of KLV format
    bit_writer.write("dshd");

    // Length of KLV format
    bit_writer.write(getLength(), 64);

    // reserved u(1)
    bit_writer.write(0, 1);

    // descriptor_ID u(7)
    bit_writer.write(descriptor_ID, 7);

    // class_ID u(4)
    bit_writer.write(class_ID, 4);

    // num_blocks u(32)
    bit_writer.write(num_blocks, 32);

    bit_writer.flush();

}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
