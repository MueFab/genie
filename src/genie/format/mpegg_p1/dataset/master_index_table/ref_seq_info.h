/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MASTER_INDEX_TABLE_REF_SEQ_INFO_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MASTER_INDEX_TABLE_REF_SEQ_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "genie/format/mpegg_p1/dataset/dataset_header.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class MITReferenceSequenceInfo {
 private:
    uint16_t ref_sequence_id;     //!< @brief
    uint64_t ref_start_position;  //!< @brief
    uint64_t ref_end_position;    //!< @brief

    DatasetHeader::Pos40SizeFlag pos_40_flag;  //!< @brief

 public:
    /**
     * @brief
     * @param reader
     */
    explicit MITReferenceSequenceInfo(util::BitReader& reader);

    /**
     * @brief
     * @param seq_id
     * @param start_pos
     * @param end_pos
     * @param flag
     */
    MITReferenceSequenceInfo(uint16_t seq_id, uint64_t start_pos, uint64_t end_pos, DatasetHeader::Pos40SizeFlag flag);

    /**
     * @brief
     * @return
     */
    uint64_t getBitLength() const;

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

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MASTER_INDEX_TABLE_REF_SEQ_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
