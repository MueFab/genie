/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "dataset.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

Dataset::Dataset(const std::unique_ptr<genie::format::mgb::DataUnitFactory>& dataUnitFactory,
                 std::vector<std::unique_ptr<genie::format::mgb::AccessUnit>>* accessUnits_p2) {
    //(void)dataUnitFactory;  // silence compiler warning

    // TODO multiple params?
    dataset_parameter_sets.push_back(util::make_unique<DatasetParameterSet>(dataUnitFactory->getParams(0)));

    for (auto& au : *accessUnits_p2) {
        access_units.push_back(util::make_unique<mpegg_p1::AccessUnit>(std::move(au)));
    }
    // TODO: add dataset_header, dataset_parametersets, accessunits(p1)
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie