/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/label_list/label/dataset_info.h"
#include <utility>
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

DatasetInfo::DatasetInfo() : dataset_ID(0), dataset_regions() {}

// ---------------------------------------------------------------------------------------------------------------------

DatasetInfo::DatasetInfo(uint16_t _ds_ID) : dataset_ID(_ds_ID) {}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetInfo::ReadDatasetInfo(util::BitReader& reader) {
    // dataset_IDs u(16)
    dataset_ID = reader.read<uint16_t>();
    // num_regions u(8)
    reader.read<uint8_t>();

    /// data encapsulated in Class dataset_region
    for (auto& ds_reg : dataset_regions) {
        ds_reg.ReadDatasetRegion(reader);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetInfo::addDatasetRegion(DatasetRegion&& _ds_region) { dataset_regions.push_back(std::move(_ds_region)); }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetInfo::setDatasetRegions(std::vector<DatasetRegion>&& _ds_regions) { dataset_regions = _ds_regions; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetInfo::addDatasetRegions(std::vector<DatasetRegion>& _ds_regions) {
    std::move(_ds_regions.begin(), _ds_regions.end(), std::back_inserter(dataset_regions));
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t DatasetInfo::getDatasetID() const { return dataset_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetInfo::getNumRegions() const { return (uint8_t)dataset_regions.size(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetInfo::getBitLength() const {
    // dataset_IDs u(16)
    uint64_t bitlen = 16;

    // num_regions u(8)
    bitlen += 8;

    /// data encapsulated in Class dataset_region
    for (auto& ds_reg : dataset_regions) {
        bitlen += ds_reg.getBitLength();
    }

    return bitlen;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetInfo::writeToFile(util::BitWriter& bit_writer) const {
    // dataset_IDs u(16)
    bit_writer.write(dataset_ID, 16);

    // num_regions u(8)
    bit_writer.write(getNumRegions(), 8);

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
