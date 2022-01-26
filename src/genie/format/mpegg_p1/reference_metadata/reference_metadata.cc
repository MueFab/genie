/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/reference_metadata/reference_metadata.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

ReferenceMetadata::ReferenceMetadata() : dataset_group_ID(0), ID(0), values() {}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceMetadata::ReferenceMetadata(uint8_t _ds_group_ID, uint8_t _ref_ID)
    : dataset_group_ID(_ds_group_ID), ID(_ref_ID), values() {}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceMetadata::ReferenceMetadata(util::BitReader& reader, FileHeader&, size_t start_pos, size_t length) {
    dataset_group_ID = reader.read<uint8_t>();
    ID = reader.read<uint8_t>();

    auto reference_metadata_value_length = length - (4 + 8) - 2;  /// K, L, dataset_groupID, ID
    values.resize(reference_metadata_value_length);

    for (auto& v : values) {
        v = reader.read<uint8_t>();
    }

    UTILS_DIE_IF(!reader.isAligned() || reader.getPos() - start_pos != length, "Invalid ReferenceMetadata length!");
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceMetadata::setDatasetGroupId(uint8_t _dataset_group_ID) { dataset_group_ID = _dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ReferenceMetadata::getLength() const {
    /// Key c(4) Length u(64)
    uint64_t len = (4 * sizeof(char) + 8);  // gen_info

    // ID u(8)
    len += 1;

    // reference_ID u(8)
    len += 1;

    // reference_metadata_value()
    len += values.size();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceMetadata::write(genie::util::BitWriter& bit_writer) const {
    // KLV (Key Length Value) format

    // Key of KLV format
    bit_writer.write("rfmd");

    // Length of KLV format
    bit_writer.write(getLength(), 64);

    // ID u(8)
    bit_writer.write(dataset_group_ID, 8);

    // reference_ID u(8)
    bit_writer.write(ID, 8);

    for (auto v : values) {
        bit_writer.write(v, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
