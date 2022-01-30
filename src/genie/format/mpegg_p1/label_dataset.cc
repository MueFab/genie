/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "label_dataset.h"
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

LabelDataset::LabelDataset(uint16_t _ds_ID) : dataset_ID(_ds_ID) {}

// ---------------------------------------------------------------------------------------------------------------------

LabelDataset::LabelDataset(util::BitReader& reader) {
    // dataset_IDs u(16)
    dataset_ID = reader.read<uint16_t>();
    // num_regions u(8)
    auto num_regions = reader.read<uint8_t>();

    /// data encapsulated in Class dataset_region
    for (uint8_t i = 0; i < num_regions; ++i) {
        dataset_regions.emplace_back(reader);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void LabelDataset::addDatasetRegion(LabelRegion _ds_region) { dataset_regions.emplace_back(std::move(_ds_region)); }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t LabelDataset::getDatasetID() const { return dataset_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t LabelDataset::getBitLength() const {
    // dataset_IDs u(16)
    uint64_t bitlen = 16;

    // num_regions u(8)
    bitlen += 8;

    /// data encapsulated in Class dataset_region
    for (auto& ds_reg : dataset_regions) {
        bitlen += ds_reg.sizeInBits();
    }

    return bitlen;
}

// ---------------------------------------------------------------------------------------------------------------------

void LabelDataset::write(util::BitWriter& bit_writer) const {
    // dataset_IDs u(16)
    bit_writer.write(dataset_ID, 16);

    // num_regions u(8)
    bit_writer.write(dataset_regions.size(), 8);

    /// data encapsulated in Class dataset_region
    for (auto& ds_reg : dataset_regions) {
        ds_reg.write(bit_writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
