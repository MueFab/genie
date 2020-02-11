#include "dataset_group_header.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

DatasetGroupHeader::DatasetGroupHeader(const std::vector<genie::format::mpegg_p1::Dataset>& datasets)
    : dataset_group_ID(0), version_number(0) {
    for (auto dataset : datasets) {
        dataset_ID.push_back(dataset.getDatasetHeader().getDatasetId());
    }
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
