/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset/class_description.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

ClassDescription::ClassDescription(genie::util::BitReader& reader, bool _block_header_flag)
    : block_header_flag(_block_header_flag), clid(reader.read<uint8_t>(4)), descriptor_IDs() {
    if (!block_header_flag) {
        auto num_descriptor = reader.read<uint8_t>(5);
        for (auto di = 0; di < num_descriptor; di++) {
            descriptor_IDs.push_back(reader.read<uint8_t>(7));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ClassDescription::getBitLength() const {
    /// clid u(4)
    uint64_t bitlen = 4;

    if (!block_header_flag) {
        /// num_descriptors u(5), descriptor_IDs[] u(7)
        bitlen += 5 + descriptor_IDs.size() * 7;
    }

    return bitlen;
}

// ---------------------------------------------------------------------------------------------------------------------

void ClassDescription::write(genie::util::BitWriter& writer) const {
    /// clid u(4)
    writer.write(clid, 4);

    if (!block_header_flag) {
        /// num_descriptors u(5)
        writer.write(descriptor_IDs.size(), 5);

        /// descriptor_IDs[] u(7)
        for (auto& descriptor_ID : descriptor_IDs) {
            writer.write(descriptor_ID, 7);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
