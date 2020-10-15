/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <genie/util/runtime-exception.h>

#include "descriptor_stream.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

DescriptorStream::DescriptorStream() {}

uint64_t DescriptorStream::getLength() const {
    uint64_t len = 12;  // gen_info

    // descriptor_stream_header
    len += descriptor_stream_header.getLength();

    // DS_protection
    len += DS_protection.getLength();

    // block_payload
    len += block_payload.size();

    return len;
}

void DescriptorStream::write(util::BitWriter& bit_writer) const {
    // Key of KVL format
    bit_writer.write("dscn");

    // Length of KVL format
    bit_writer.write(getLength(), 64);

    descriptor_stream_header.write(bit_writer);

    // DS_protection
    DS_protection.write(bit_writer);

    // block_payload
    for (auto data: block_payload){
        bit_writer.write(data, 8);
    }
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

