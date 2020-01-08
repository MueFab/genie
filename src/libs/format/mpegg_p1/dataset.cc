#include "dataset.h"

namespace format {
    namespace mpegg_p1{

std::unique_ptr<format::mpegg_p1::Dataset> Dataset::createFromDataUnits(
    const std::vector<std::unique_ptr<format::mpegg_p2::DataUnit>> * const dataUnits) {
    (void)dataUnits;  // silence compiler warning

            return nullptr;
        }
    }//namespace mpegg_p1
}// namespace format