/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "dataset_parameter_set.h"

namespace genie {
namespace format {
namespace mpegg_p1 {
DatasetParameterSet::DatasetParameterSet(core::parameter::ParameterSet& parameterSet)
    : core::parameter::ParameterSet(parameterSet) {}
}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie