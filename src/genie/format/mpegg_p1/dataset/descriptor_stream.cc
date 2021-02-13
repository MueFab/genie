/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <genie/util/runtime-exception.h>
#include <genie/format/mpegg_p1/util.h>

#include "descriptor_stream.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------
DSProtection::DSProtection()
    : DS_protection_value() {}

DSProtection::DSProtection(util::BitReader& bit_reader, size_t length)
    : DS_protection_value() {

    size_t start_pos = bit_reader.getPos();

    std::string key = readKey(bit_reader);
    UTILS_DIE_IF(key != "dspr", "DSProtection is not Found");

    // DS_protection_value std::vector<uint8_t>
    for (auto& val : DS_protection_value) {
        bit_reader.read<uint8_t>();
    }

    UTILS_DIE_IF(bit_reader.getPos() - start_pos != length, "Invalid DSProtection length!");
}


uint64_t DSProtection::getLength() const {

    /// Key c(4) Length u(64)
    uint64_t len = (4 * sizeof(char) + 8);   // gen_info

    // DS_protection_value[] std::vector<uint8_t>
    len += DS_protection_value.size();

    return len;
}


void DSProtection::write(genie::util::BitWriter &bit_writer) const {

    /// KLV (Key Length Value) format
    bit_writer.write("dspr");

    // Length of KLV format
    bit_writer.write(getLength(), 64);

    for (auto& val : DS_protection_value){
        bit_writer.write(val, 8);
    }
}


// ---------------------------------------------------------------------------------------------------------------------

DescriptorStream::DescriptorStream()
    : descriptor_stream_header(),
      DS_protection(),
      block_payload() {}


DescriptorStream::DescriptorStream(genie::util::BitReader& bit_reader, size_t length)
    : descriptor_stream_header(),
      DS_protection(),
      block_payload() {

    size_t start_pos = bit_reader.getPos();

    std::string key = readKey(bit_reader);
    UTILS_DIE_IF(key != "dscn", "DescriptorStream is not Found");
    // TODO(Raouf) : check

    /// descriptor_stream_header
    auto header_length = bit_reader.read<size_t>();
    descriptor_stream_header = DescriptorStreamHeader(bit_reader,  header_length);
    /// DS_protection
    auto DS_length = bit_reader.read<size_t>();
    DS_protection = DSProtection(bit_reader, DS_length);

    /// block_payload
    for (auto block : block_payload) {
        block_payload.emplace_back(bit_reader.read<uint8_t>());
    }

    bit_reader.flush();

    UTILS_DIE_IF(bit_reader.getPos() - start_pos != length, "Invalid DescriptorStream length!");
}

uint64_t DescriptorStream::getLength() const {

    /// Key c(4) Length u(64)
    uint64_t len = (4 * sizeof(char) + 8);   // gen_info

    // descriptor_stream_header
    len += descriptor_stream_header.getLength();

    // DS_protection
    len += DS_protection.getLength();

    // block_payload
    len += block_payload.size();

    return len;
}

void DescriptorStream::write(util::BitWriter& bit_writer) const {
    /// Key of KLV format
    bit_writer.write("dscn");

    /// Length of KLV format
    bit_writer.write(getLength(), 64);

    // descriptor_stream_header
    descriptor_stream_header.write(bit_writer);

    // DS_protection
    DS_protection.write(bit_writer);

    // block_payload
    for (auto& data: block_payload){
        bit_writer.write(data, 8);
    }

    bit_writer.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

