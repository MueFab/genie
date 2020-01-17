/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "dataset_parameter_set.h"

namespace genie {
namespace format {
namespace mpegg_p1 {
DatasetParameterSet::DatasetParameterSet(const core::parameter::ParameterSet& parameterSet){
    parameterSet_p2 = parameterSet;
    //TODO: add dataset_group_ID and dataset_ID
}
}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie