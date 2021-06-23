/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_PARAMETER_SET_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/parameter/parameter_set.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DatasetParameterSet {
 private:
    uint8_t dataset_group_ID : 8;  //!< @brief
    uint16_t dataset_ID : 16;      //!< @brief

    core::parameter::ParameterSet parameterSet_p2;  //!< @brief

 public:
    /**
     * @brief
     */
    explicit DatasetParameterSet(const core::parameter::ParameterSet&&);
    /**
     * @brief
     * @param group_ID
     * @param ID
     */
    explicit DatasetParameterSet(uint8_t group_ID, uint16_t ID, const core::parameter::ParameterSet&& parameterSet);
    /**
     * @brief
     * @param ID
     */
    explicit DatasetParameterSet(uint16_t ID, const core::parameter::ParameterSet&&);
    /**
     * @brief
     * @param bit_reader
     * @param length
     */
    DatasetParameterSet(genie::util::BitReader& bit_reader, size_t length);

    /**
     * @brief
     * @param datasetId
     */
    void setDatasetID(uint16_t datasetID);

    /**
     * @brief
     * @return
     */
    uint16_t getDatasetID() const;

    /**
     * @brief
     * @return
     */
    uint8_t getDatasetGroupID() const;

    /**
     * @brief
     * @param datasetGroupId
     */
    void setDatasetGroupID(uint8_t datasetGroupID);

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
