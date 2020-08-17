/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "dataset_group.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

// only for single vector of Datasets -> dataset_group_ID(0)
DatasetGroup::DatasetGroup(std::vector<genie::format::mpegg_p1::Dataset>* x_datasets, const uint8_t x_datasetGroupID)
    : dataset_group_header(x_datasets, x_datasetGroupID) {
    datasets = std::move(*x_datasets);

    for (auto& dataset : datasets) {  // setting GroupIDs of Datasets
        dataset.setDatasetGroupId(x_datasetGroupID);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::writeToFile(genie::util::BitWriter& bitWriter) const {
    bitWriter.write("dgcn");

    uint64_t length = 12;                                          //"dgcn" dataset_group
    length += 12 + 2 + 2 * dataset_group_header.getNumDatasets();  //"dghd" dataset_group_header
    for (auto const& it : datasets) {
        length += it.getLength();
    }

    bitWriter.write(length, 64);

    dataset_group_header.writeToFile(bitWriter);
    for (auto const& it : datasets) {
        it.writeToFile(bitWriter);
    }

    bitWriter.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

DG_metadata::DG_metadata() : DG_metadata_value() {
    DG_metadata_value = {0x37, 0xfd, 0x58, 0x7a, 0x00, 0x5a, 0x04, 0x00, 0xd6, 0xe6, 0x46,
                         0xb4, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x1c, 0x21, 0x44, 0xb6, 0x1f,
                         0x7d, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x5a, 0x59};
}

// ---------------------------------------------------------------------------------------------------------------------

DG_protection::DG_protection() : DG_protection_value() {
    DG_protection_value = {0x37, 0xfd, 0x58, 0x7a, 0x00, 0x5a, 0x04, 0x00, 0xd6, 0xe6, 0x46,
                           0xb4, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x1c, 0x21, 0x44, 0xb6, 0x1f,
                           0x7d, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x5a, 0x59};
}

// ---------------------------------------------------------------------------------------------------------------------

const DatasetGroupHeader& DatasetGroup::getDatasetGroupHeader() const { return dataset_group_header; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Dataset>& DatasetGroup::getDatasets() const { return datasets; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------