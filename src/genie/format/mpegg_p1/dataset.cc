/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "dataset.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

Dataset::Dataset(const genie::format::mgb::DataUnitFactory& dataUnitFactory,
                 std::vector<genie::format::mgb::AccessUnit>& accessUnits_p2) {
    // TODO multiple params?
    for (unsigned int i = 0;; ++i) {
        try {
            dataset_parameter_sets.emplace_back(dataUnitFactory.getParams(i));
        } catch (const std::out_of_range&) {
            std::cout << "Got " << i << " ParameterSet/s from DataUnitFactory" << std::endl;
            break;
        }
    }

    for (auto& au : accessUnits_p2) {
        access_units.emplace_back(std::move(au));
    }
    // TODO: add dataset_header, dataset_parametersets, accessunits(p1)
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie