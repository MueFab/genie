/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_PARAMETER_SET_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include <memory>
#include "genie/util/make-unique.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/format/mpegg_p1/dataset/class_description.h"

#include "genie/core/parameter/parameter_set.h"
#include <sstream>
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DatasetParameterSet: public core::parameter::ParameterSet {
 private:
    uint8_t group_ID;  //!< @brief
    uint16_t ID;      //!< @brief

 public:
    /**
     * @brief
     * @param group_ID
     * @param ID
     * @param parameterSet
     */
    explicit DatasetParameterSet(uint8_t _group_ID, uint16_t _ID, const core::parameter::ParameterSet&& p2_param_set);

    /**
     * @brief
     * @param bit_reader
     * @param length
     */
    DatasetParameterSet(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length);

    /**
     * @brief
     * @return
     */
    uint8_t getDatasetGroupID() const;

    /**
     * @brief
     * @param datasetGroupID
     */
    void setDatasetGroupID(uint8_t datasetGroupID);

    /**
     * @brief
     * @return
     */
    uint16_t getDatasetID() const;

    /**
     * @brief
     * @param datasetID
     */
    void setDatasetID(uint16_t datasetID);

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
