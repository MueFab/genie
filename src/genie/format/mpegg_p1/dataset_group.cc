#include "dataset_group.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// only for single vector of Datasets -> dataset_group_ID(0)
DatasetGroup::DatasetGroup(std::vector<genie::format::mpegg_p1::Dataset>* x_datasets)
    : dataset_group_header(x_datasets) {
    datasets = std::move(*x_datasets);

    for (auto& dataset : datasets) {  // setting GroupIDs of Datasets
        dataset.setDatasetHeaderGroupId(0);
        dataset.setDatasetParameterSetsGroupId(0);
    }
}

void DatasetGroup::writeToFile(genie::util::BitWriter& bitWriter) const {
    bitWriter.write("dgcn");

    uint64_t length = 12;                                           //"dgcn" dataset_group
    length += 12 + 2 + 2 * dataset_group_header.getNumDatasets();   //"dghd" dataset_group_header
    for (auto const& it: datasets) {
        length += it.getLength();
    }

    bitWriter.write(length, 64);
    // TODO
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie