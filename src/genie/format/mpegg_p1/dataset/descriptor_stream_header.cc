/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <genie/util/runtime-exception.h>

#include "descriptor_stream_header.h"

namespace genie {
namespace format {
namespace mpegg_p1 {


uint64_t DescriptorStreamHeader::getLength() const {

  //length is first calculated in bits then converted in bytes

       // Key c(4) Length u(64)
       uint64_t bitlength = (4 * sizeof(char) + 8) * 8 ;   // gen_info

       //reserved u(1)
        bitlength += 1;

       //descriptor_ID u(7)
       bitlength += 7 ;

       //class_ID u(4)
       bitlength += 4 ;

       //num_blocks u(32)
       bitlength += 32 ;

    // !byte_aligned()
    bitlength += bitlength % 8;

    bitlength /= 8;  // byte conversion

    return bitlength;
}

void DescriptorStreamHeader::write(util::BitWriter& bit_writer) const {

    bit_writer.write("dshd");

    // Length of KVL format
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
