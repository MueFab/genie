/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MASTER_INDEX_TABLE_ACCESS_UNIT_INFO_ACCESS_UNIT_INFO_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MASTER_INDEX_TABLE_ACCESS_UNIT_INFO_ACCESS_UNIT_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <vector>
#include "genie/format/mpegg_p1/dataset/master_index_table/access_unit_info/extended_au_info.h"
#include "genie/format/mpegg_p1/dataset/master_index_table/ref_seq_info.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class MITAccessUnitInfo {
 private:
    uint64_t AU_byte_offset;     //!< @brief
    uint64_t AU_start_position;  //!< @brief
    uint64_t AU_end_position;    //!< @brief

    std::unique_ptr<MITReferenceSequenceInfo> ref_seq_info;  //!< @brief

    std::unique_ptr<MITExtendedAUInfo> extended_au_info;  //!< @brief

    std::vector<uint64_t> block_byte_offset;  //!< @brief

    DatasetHeader* datasetHeader;                             //!< @brief
    DatasetHeader::ByteOffsetSizeFlag byte_offset_size_flag;  //!< @brief
    DatasetHeader::Pos40SizeFlag pos_40_size_flag;            //!< @brief

 public:
    /**
     * @brief
     */
    MITAccessUnitInfo();

    /**
     * @brief
     * @param _AU_byte_offset
     * @param _AU_start_position
     * @param _AU_end_position
     * @param _block_byte_offset
     * @param _datasetHeader
     * @param _byte_offset_size_flag
     * @param _pos_40_size_flag
     */
    MITAccessUnitInfo(uint64_t _AU_byte_offset, uint64_t _AU_start_position, uint64_t _AU_end_position,
                      std::vector<uint64_t>&& _block_byte_offset, DatasetHeader* _datasetHeader,
                      DatasetHeader::ByteOffsetSizeFlag _byte_offset_size_flag,
                      DatasetHeader::Pos40SizeFlag _pos_40_size_flag);

    /**
     * @brief
     * @param reader
     */
    explicit MITAccessUnitInfo(util::BitReader& reader);

    /**
     * @brief
     * @return
     */
    uint64_t getAUbyteOffset() const;

    /**
     * @brief
     * @return
     */
    const std::vector<uint64_t>& getBlockByteOffset() const;

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

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_MASTER_INDEX_TABLE_ACCESS_UNIT_INFO_ACCESS_UNIT_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
