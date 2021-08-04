/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MASTER_INDEX_TABLE_U_ACCESS_UNIT_INFO_U_ACCESS_UNIT_INFO_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MASTER_INDEX_TABLE_U_ACCESS_UNIT_INFO_U_ACCESS_UNIT_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "genie/format/mpegg_p1/dataset/dataset_header.h"
#include "genie/format/mpegg_p1/dataset/master_index_table/access_unit_info/access_unit_info.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class MITUAccessUnitInfo {
 private:
    uint32_t multiple_signature_base;  //!< @brief
    uint8_t U_signature_size;          //!< @brief
    bool U_signature_constant_length;  //!< @brief
    uint8_t U_signature_length;        //!< @brief

    std::vector<uint8_t> U_cluster_signature;  //!< @brief
    uint16_t num_signatures;                   //!< @brief

    DatasetHeader* datasetHeader;  //!< @brief

    uint16_t U_ref_sequence_id;     //!< @brief
    uint64_t U_ref_start_position;  //!< @brief
    uint64_t U_ref_end_position;    //!< @brief

    DatasetHeader::ByteOffsetSize byte_offset_size_flag;  //!< @brief
    DatasetHeader::Pos40Size pos_40_size_flag;            //!< @brief

 public:
    /**
     * @brief
     */
    MITUAccessUnitInfo();

    /**
     * @brief
     * @param _num_U_clusters
     */
    explicit MITUAccessUnitInfo(uint32_t _num_U_clusters);

    /**
     * @brief
     * @param reader
     */
    explicit MITUAccessUnitInfo(util::BitReader& reader);
    /**
     * @brief
     * @param base
     * @param size
     */
    void setMultipleSignature(uint32_t base, uint8_t size);
    /**
     * @brief
     * @param sign_length
     */
    void setConstantSignature(uint8_t sign_length);
    /**
     * @brief
     * @return
     */
    uint8_t getUSignatureSize() const;
    /**
     * @brief
     * @return
     */
    uint32_t getMultipleSignatureBase() const;

    /**
     * @brief
     * @return
     */
    uint64_t getBitLength() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MASTER_INDEX_TABLE_U_ACCESS_UNIT_INFO_U_ACCESS_UNIT_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
