/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/label_dataset.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

bool LabelDataset::operator==(const LabelDataset& other) const {
    return dataset_ID == other.dataset_ID && dataset_regions == other.dataset_regions;
}

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

std::vector<genie::core::meta::Region> LabelDataset::decapsulate(uint16_t dataset) {
    std::vector<genie::core::meta::Region> ret;
    if (dataset != dataset_ID) {
        return ret;
    }
    for (auto& r : dataset_regions) {
        ret.emplace_back(r.decapsulate());
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

LabelDataset::LabelDataset(uint16_t _dataset_ID, genie::core::meta::Label& labels) : dataset_ID(_dataset_ID) {
    for (auto& l : labels.getRegions()) {
        dataset_regions.emplace_back(l.getSeqID(), l.getStartPos(), l.getEndPos());
        for (auto& c : l.getClasses()) {
            dataset_regions.back().addClassID(c);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
