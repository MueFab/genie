#include "dataset.h"

namespace format {
    namespace mpegg_p1{

std::unique_ptr<format::mpegg_p1::Dataset> Dataset::createFromDataUnits(
    const std::vector<std::unique_ptr<format::mpegg_p2::DataUnit>>& dataUnits) {
    return std::unique_ptr<Dataset>(new Dataset(dataUnits));  // TODO: direct call of constructor instead of factory?
}

// TODO: implement constructor
Dataset::Dataset(const std::vector<std::unique_ptr<format::mpegg_p2::DataUnit>>& dataUnits) {
    (void)dataUnits;  // silence compiler warning
}
}  // namespace mpegg_p1
}  // namespace format