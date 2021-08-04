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

#include "genie/core/parameter/parameter_set.h"
#include "genie/format/mpegg_p1/dataset/dataset_header.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DatasetParameterSet: public core::parameter::ParameterSet {
 private:
    std::string minor_version;
    bool parameters_update_flag;
    uint32_t num_U_access_units;

    uint8_t dataset_group_ID;
    uint16_t dataset_ID;
//    uint8_t ID;
//    uint8_t parent_ID;

    bool multiple_alignment_flag;
    DatasetHeader::Pos40Size pos_40_bits_flag;
    uint8_t alphabet_ID;
    bool U_signature_flag;
    bool U_signature_constant_length;
    uint8_t U_signature_length;


 public:
    /**
     * @brief
     * @param group_ID
     * @param ID
     * @param parameterSet
     */
//    explicit DatasetParameterSet(uint8_t _group_ID, uint16_t _ID, const core::parameter::ParameterSet&& p2_param_set);

    /**
     * @brief
     * @param bit_reader
     * @param length
     */
    DatasetParameterSet(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length, DatasetHeader& dthd);

    /**
     * @brief
     * @return
     */
    uint8_t getDatasetGroupID() const;

    /**
     * @brief
     * @param datasetGroupID
     */
    void setDatasetGroupID(uint8_t _dataset_group_ID);

    /**
     * @brief
     * @return
     */
    uint16_t getDatasetID() const;

    /**
     * @brief
     * @param datasetID
     */
    void setDatasetID(uint16_t _dataset_ID);

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer, bool empty_length=false) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
