#include "dataset_group_header.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

DatasetGroupHeader::DatasetGroupHeader(const std::vector<genie::format::mpegg_p1::Dataset>* datasets, const uint8_t x_datasetGroupID)
    : dataset_group_ID(x_datasetGroupID), version_number(0) {
    for (const auto& dataset : *datasets) {
        dataset_ID.push_back(dataset.getDatasetHeader().getDatasetId());
    }
}

void DatasetGroupHeader::writeToFile(genie::util::BitWriter& bitWriter) const
{
    bitWriter.write("dghd");

    uint64_t length = 12; //key + length
    length += 2 + 2 * this->getNumDatasets();
    bitWriter.write(length, 64);

    bitWriter.write(dataset_group_ID, 8);
    bitWriter.write(version_number, 8);
    for(auto const& it: dataset_ID) {
        bitWriter.write(it, 16);
    }

    bitWriter.flush();
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
