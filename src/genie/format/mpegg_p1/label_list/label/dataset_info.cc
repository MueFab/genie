
#include "dataset_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

DatasetInfo::DatasetInfo(uint16_t _ds_ID)
    : dataset_ID(_ds_ID){}

void DatasetInfo::addDatasetRegion(DatasetRegion&& _ds_region) {
    dataset_regions.push_back(std::move(_ds_region));
}

void DatasetInfo::setDatasetRegions(std::vector<DatasetRegion>&& _ds_regions) {
    dataset_regions = _ds_regions;
}

void DatasetInfo::addDatasetRegions(std::vector<DatasetRegion>& _ds_regions) {
    std::move(_ds_regions.begin(), _ds_regions.end(), std::back_inserter(dataset_regions));
}

uint16_t DatasetInfo::getDatasetID() const { return dataset_ID; }

uint8_t DatasetInfo::getNumRegions() const { return (uint8_t) dataset_regions.size(); }

const std::vector<DatasetRegion>& DatasetInfo::getDatasetRegions() const { return dataset_regions; }

void DatasetInfo::write(util::BitWriter& bit_writer) const {

    // dataset_IDs u(16)
    bit_writer.write(dataset_ID, 16);

    // num_regions u(8)
    bit_writer.write(getNumRegions(), 8);

    // for dataset_regions
    for (auto& dataset_region: dataset_regions){
        dataset_region.write(bit_writer);
    }
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
