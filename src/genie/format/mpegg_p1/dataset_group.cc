#include "dataset_group.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// only for single vector of Datasets -> dataset_group_ID(0)
DatasetGroup::DatasetGroup(std::vector<genie::format::mpegg_p1::Dataset>* x_datasets, const uint8_t x_datasetGroupID)
    : dataset_group_header(x_datasets, x_datasetGroupID) {
    datasets = std::move(*x_datasets);

    for (auto& dataset : datasets) {  // setting GroupIDs of Datasets
        dataset.setDatasetGroupId(x_datasetGroupID);
    }
}

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

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie