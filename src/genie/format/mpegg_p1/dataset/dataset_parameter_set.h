/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_PART1_DATASET_PARAMETER_SET_H
#define GENIE_PART1_DATASET_PARAMETER_SET_H

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

    // parameter_set_ID and parent_parameter_set_ID also encoded_parameters()
    core::parameter::ParameterSet parameterSet_p2;  //!<

   public:
    /**
     *
     */
    explicit DatasetParameterSet(const core::parameter::ParameterSet&&);

    /**
     *
     * @param ID
     */
    explicit DatasetParameterSet(uint16_t ID, const core::parameter::ParameterSet&&);

    /**
     *
     * @param group_ID
     * @param ID
     */
    explicit DatasetParameterSet(uint8_t group_ID, uint16_t ID,
                                 const core::parameter::ParameterSet&& parameterSet);

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
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_PART1_DATASET_PARAMETER_SET_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------