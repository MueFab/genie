/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DATASET_PARAMETER_SET_H
#define GENIE_DATASET_PARAMETER_SET_H

#include <genie/core/parameter/parameter_set.h>

namespace genie {
namespace format {
namespace mpegg_p1 {
class DatasetParameterSet {
   public:
    explicit DatasetParameterSet(const core::parameter::ParameterSet&);

   private:
    /**
     * ISO 23092-1 Section 6.5.2 table 23
     *
     * ------------------------------------------------------------------------------------------------------------- */
    uint8_t dataset_group_ID : 8;
    uint16_t dataset_ID : 16;

    //std::unique_ptr<core::parameter::ParameterSet> parameterSet_p2;
    core::parameter::ParameterSet parameterSet_p2;
};
}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PARAMETER_SET_H
