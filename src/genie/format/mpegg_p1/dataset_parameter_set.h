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
    explicit DatasetParameterSet(const core::parameter::ParameterSet&&);

    explicit DatasetParameterSet(const core::parameter::ParameterSet&&, const uint8_t x_dataset_group_ID,
                                 const uint16_t x_dataset_ID);

    explicit DatasetParameterSet(const core::parameter::ParameterSet&&, const uint16_t x_dataset_ID);

    uint16_t getDatasetID() const { return dataset_ID; }
    uint8_t getDatasetGroupID() const { return dataset_group_ID; }

    void setDatasetGroupId(uint8_t datasetGroupId) { dataset_group_ID = datasetGroupId; }

    const uint64_t getLength() const;

   private:
    /**
     * ISO 23092-1 Section 6.5.2 table 23
     *
     * ------------------------------------------------------------------------------------------------------------- */
    uint8_t dataset_group_ID : 8;
    uint16_t dataset_ID : 16;

    core::parameter::ParameterSet parameterSet_p2;
};
}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PARAMETER_SET_H
