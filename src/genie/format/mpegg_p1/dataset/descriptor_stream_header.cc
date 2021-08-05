/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset/descriptor_stream_header.h"
#include <sstream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStreamHeader::DescriptorStreamHeader() : descriptor_ID(0),
                                                   class_ID(core::record::ClassType::NONE),
                                                   num_blocks(0) {}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStreamHeader::DescriptorStreamHeader(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length) {
    /// reserved u(1)
    reader.read_b(1);

    /// descriptor_ID u(7)
    descriptor_ID = reader.read<uint8_t>(7);

    /// class_ID u(4)
    class_ID = reader.read<core::record::ClassType>(4);

    /// num_blocks u(32)
    num_blocks = reader.read<uint32_t>();

    reader.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DescriptorStreamHeader::getLength() const {
    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    write(tmp_writer, true);
    tmp_writer.flush();
    return tmp_writer.getBitsWritten() / 8;
}

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorStreamHeader::write(util::BitWriter& writer, bool zero_length) const {
    // Key of KLV format
    writer.write("dshd");

    /// Length of KLV format
    if (zero_length){
        writer.write(0, 64);
    } else {
        writer.write(getLength(), 64);
    }

    /// reserved u(1)
    writer.write(0, 1);

    /// descriptor_ID u(7)
    writer.write(descriptor_ID, 7);

    /// class_ID u(4)
    writer.write((uint8_t)class_ID, 4);

    /// num_blocks u(32)
    writer.write(num_blocks, 32);

    writer.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
