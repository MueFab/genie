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

DatasetGroupHeader::DatasetGroupHeader()
    : dataset_group_ID(),
      version_number(){}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroupHeader::DatasetGroupHeader(std::vector<Dataset> &datasets, uint8_t ver)
    : version_number(ver) {

    auto iter = datasets.begin();

    dataset_group_ID = iter->getDatasetHeader().getDatasetGroupId();
    dataset_IDs.push_back(iter->getDatasetParameterSetDatasetID());

    iter++;

    while (iter != datasets.end()){
        UTILS_DIE_IF(iter->getDatasetHeader().getDatasetGroupId() != dataset_group_ID,
                     "dataset_group_ID is inconsistent");

        dataset_IDs.push_back(iter->getDatasetParameterSetDatasetID());

        iter++;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupHeader::setDatasetGroupId(uint8_t _dataset_group_ID) {dataset_group_ID = _dataset_group_ID;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetGroupHeader::getDatasetGroupId() const { return dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupHeader::setVersionNumber(uint8_t _version_number) {version_number = _version_number;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetGroupHeader::getVersionNumber() const { return version_number; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetGroupHeader::getNumDatasets() const { return dataset_IDs.size(); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint16_t>& DatasetGroupHeader::getDatasetId() const { return dataset_IDs; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetGroupHeader::getLength() const {
    // key (4), Length (8)
    uint64_t len = 12;

    // VALUE

    // dataset_group_ID u(8)
    len += 1;

    // version_number u(8)
    len += 1;

    // dataset_IDs[] u(16)
    len += 2 * dataset_IDs.size();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupHeader::write(genie::util::BitWriter& bit_writer) const {
    // KLV (Key Length Value) format

    // Key of KVL format
    bit_writer.write("dghd");

    // Length of KVL format
    bit_writer.write(getLength(), 64);

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

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------