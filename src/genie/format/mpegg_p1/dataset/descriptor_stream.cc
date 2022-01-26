/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset/descriptor_stream.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

DSProtection::DSProtection() : DS_protection_value() {}

// ---------------------------------------------------------------------------------------------------------------------

DSProtection::DSProtection(util::BitReader& bit_reader, size_t length) : DS_protection_value() {
    std::string key = readKey(bit_reader);
    UTILS_DIE_IF(key != "dspr", "DSProtection is not Found");

    size_t start_pos = bit_reader.getPos();

    // DS_protection_value[uint8_t]
    for (auto& DS_val : DS_protection_value) {
        DS_val = bit_reader.read<uint8_t>();
    }

    UTILS_DIE_IF(bit_reader.getPos() - start_pos != length, "Invalid DSProtection length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DSProtection::getLength() const {
    /// Key c(4) Length u(64)
    uint64_t len = (4 * sizeof(char) + 8);  // gen_info

    // DS_protection_value[] std::vector<uint8_t>
    len += DS_protection_value.size();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void DSProtection::write(genie::util::BitWriter& bit_writer) const {
    /// KLV (Key Length Value) format
    // Key of KLV format
    bit_writer.write("dspr");

    // Length of KLV format
    bit_writer.write(getLength(), 64);

    for (auto& DS_val : DS_protection_value) {
        bit_writer.write(DS_val, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStream::DescriptorStream() : header(), DS_protection(), block_payload() {}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStream::DescriptorStream(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length)
    : minor_version(fhd.getMinorVersion()) {
    /// access_unit_header
    size_t box_start_pos = reader.getPos();
    std::string box_key = readKey(reader);
    auto box_length = reader.read<uint64_t>();
    UTILS_DIE_IF(box_key != "dshd", "DescriptorStreamHeader is not found!");

    header = DescriptorStreamHeader(reader, fhd, box_start_pos, box_length);

    do {
        /// Read K,L of KLV
        box_start_pos = reader.getPos();
        box_key = readKey(reader);
        box_length = reader.read<uint64_t>();

        /// AU_information
        if (box_key == "auin") {
        } else if (box_key == "aupr") {
        } else {
        }
    } while (reader.getPos() - start_pos < length);
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DescriptorStream::getLength() const {
    /// Key c(4) Length u(64)
    uint64_t len = (4 * sizeof(char) + 8);  // gen_info

    /// descriptor_stream_header
    len += header.getLength();

    /// DS_protection
    len += DS_protection.getLength();

    // block_payload
    len += block_payload.size();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorStream::write(util::BitWriter& bit_writer) const {
    /// Key of KLV format
    bit_writer.write("dscn");

    /// Length of KLV format
    bit_writer.write(getLength(), 64);

    /// descriptor_stream_header
    header.write(bit_writer);

    /// DS_protection
    DS_protection.write(bit_writer);

    /// block_payload
    for (auto& data : block_payload) {
        bit_writer.write(data, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
