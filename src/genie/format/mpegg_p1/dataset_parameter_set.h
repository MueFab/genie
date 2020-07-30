/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DATASET_PARAMETER_SET_H
#define GENIE_DATASET_PARAMETER_SET_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/parameter/parameter_set.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 *
 */
class DatasetParameterSet {

   private:
    uint8_t dataset_group_ID : 8;  //!<
    uint16_t dataset_ID : 16;      //!<

    core::parameter::ParameterSet parameterSet_p2;  //!<

   public:
    /**
     *
     */
    explicit DatasetParameterSet(const core::parameter::ParameterSet&&);

    /**
     *
     * @param x_dataset_group_ID
     * @param x_dataset_ID
     */
    explicit DatasetParameterSet(const core::parameter::ParameterSet&&, const uint8_t x_dataset_group_ID,
                                 const uint16_t x_dataset_ID);

    /**
     *
     * @param x_dataset_ID
     */
    explicit DatasetParameterSet(const core::parameter::ParameterSet&&, const uint16_t x_dataset_ID);

    /**
     *
     * @return
     */
    uint16_t getDatasetID() const;

    /**
     *
     * @return
     */
    uint8_t getDatasetGroupID() const;

    /**
     *
     * @param datasetGroupId
     */
    void setDatasetGroupId(uint8_t datasetGroupId);

    /**
     *
     * @return
     */
    uint64_t getLength() const;

    /**
     *
     * @param bitWriter
     */
    void writeToFile(genie::util::BitWriter& bitWriter) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_PARAMETER_SET_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------