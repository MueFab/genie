/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "dataset_group_header.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroupHeader::DatasetGroupHeader(const std::vector<genie::format::mpegg_p1::Dataset>* datasets,
                                       const uint8_t x_datasetGroupID)
    : dataset_group_ID(x_datasetGroupID), version_number(0) {
    for (const auto& ds : *datasets) {
        dataset_IDs.push_back(ds.getDatasetHeader().getDatasetId());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupHeader::writeToFile(genie::util::BitWriter& bit_writer) const {
    bit_writer.write("dghd");

    uint64_t length = 12;  // key + length
    length += 2 + 2 * this->getNumDatasets();
    bit_writer.write(length, 64);

    // dataset_group_ID u(8)
    bit_writer.write(dataset_group_ID, 8);

    // version_number u(8)
    bit_writer.write(version_number, 8);

    // dataset_IDs[] u(16)
    for (auto &d_ID: dataset_IDs){
        bit_writer.write(d_ID, 16);
    }

    bit_writer.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetGroupHeader::getDatasetGroupId() const { return dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetGroupHeader::getVersionNumber() const { return version_number; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetGroupHeader::getNumDatasets() const { return dataset_IDs.size(); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint16_t>& DatasetGroupHeader::getDatasetId() const { return dataset_IDs; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------