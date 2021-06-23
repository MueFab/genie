/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "genie/core/parameter/data_unit.h"
#include "genie/format/mpegg_p1/dataset/fields/block_header_flags/block_header_flags.h"
#include "genie/format/mpegg_p1/dataset/fields/seq_info/seq_info.h"
#include "genie/format/mpegg_p1/dataset/fields/u_access_unit_info/u_access_unit_info.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DatasetHeader {
 public:
    /**
     * @brief
     */
    enum class ByteOffsetSizeFlag : uint8_t {
        ON = 64,
        OFF = 32,
    };

    /**
     * @brief
     */
    enum class Pos40SizeFlag : uint8_t {
        ON = 40,
        OFF = 32,
    };

 private:
    /** ------------------------------------------------------------------------------------------------------------
     *  ISO 23092-1 Section 6.5.2 table 19
     *  ------------------------------------------------------------------------------------------------------------ */
    uint8_t dataset_group_ID;                  //!< @brief
    uint16_t dataset_ID;                       //!< @brief
    std::string version;                       //!< @brief
    ByteOffsetSizeFlag byte_offset_size_flag;  //!< @brief
    bool non_overlapping_AU_range_flag;        //!< @brief
    Pos40SizeFlag pos_40_bits_flag;            //!< @brief
    bool multiple_alignment_flag;              //!< @TODO(Raouf): check existence of variable

    BlockConfig block_header;  //!< @brief block_header_flag, MIT_flag, CC_mode_flag, ordered_blocks_flag, ClassInfo
                               //!< (num_classes, clid[], num_descriptors[], descriptor_ID[][])

    SequenceConfig seq_info;  //!< @brief seq_count, reference_ID, seq_ID[], seq_blocks[], tflag[], thres[]

    core::parameter::DataUnit::DatasetType dataset_type;  //!< @brief

    uint8_t alphabet_ID;          //!< @brief
    uint32_t num_U_access_units;  //!< @brief

    std::unique_ptr<UAccessUnitInfo>
        u_access_unit_info;  //!< @brief num_U_clusters, multiple_signature_base, U_signature_size,
                             //!< U_signature_constant_length, U_signature_length

 public:
    /**
     * @brief
     */
    DatasetHeader();

    /**
     * @brief
     * @param datasetID
     */
    explicit DatasetHeader(uint16_t datasetID);

    /**
     * @brief
     * @param group_ID
     * @param ID
     * @param _byte_offset_size_flag
     * @param _non_overlapping_AU_range_flag
     * @param _pos_40_bits_flag
     * @param _multiple_alignment_flag
     * @param _dataset_type
     * @param _alphabet_ID
     * @param _num_U_access_units
     */
    DatasetHeader(uint8_t group_ID, uint16_t ID, ByteOffsetSizeFlag _byte_offset_size_flag,
                  bool _non_overlapping_AU_range_flag, Pos40SizeFlag _pos_40_bits_flag, bool _multiple_alignment_flag,
                  core::parameter::DataUnit::DatasetType _dataset_type, uint8_t _alphabet_ID,
                  uint32_t _num_U_access_units);

    /**
     * @brief
     * @param bit_reader
     * @param length
     */
    DatasetHeader(genie::util::BitReader& bit_reader, size_t length);

    /**
     * @brief
     * @return
     */
    uint16_t getID() const;

    /**
     * @brief
     * @param ID
     */
    void setID(uint16_t ID);

    /**
     * @brief
     * @return
     */
    uint8_t getGroupID() const;

    /**
     * @brief
     * @param group_ID
     */
    void setGroupId(uint8_t group_ID);

    /**
     * @brief
     * @return
     */
    ByteOffsetSizeFlag getByteOffsetSizeFlag() const;

    /**
     * @brief
     * @return
     */
    Pos40SizeFlag getPos40SizeFlag() const;

    /**
     * @brief
     * @return
     */
    const SequenceConfig& getSeqInfo() const;

    /**
     * @brief
     * @return
     */
    const BlockConfig& getBlockHeader() const;

    /**
     * @brief
     * @return
     */
    uint32_t getNumUAccessUnits() const;

    /**
     * @brief
     * @return
     */
    bool getMultipleAlignmentFlag() const;

    /**
     * @brief
     * @return
     */
    core::parameter::DataUnit::DatasetType getDatasetType() const;

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

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
