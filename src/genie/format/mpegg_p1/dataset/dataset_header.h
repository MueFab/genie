/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include <memory>
#include "genie/util/make-unique.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/format/mpegg_p1/dataset/class_description.h"

#include <cstdint>
#include <iostream>
#include <genie/core/constants.h>
#include "genie/core/parameter/data_unit.h"
#include "u_access_unit_info.h"

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
    enum class ByteOffsetSize : uint8_t {
        ON = 64,
        OFF = 32,
    };

    /**
     * @brief
     */
    enum class Pos40Size : uint8_t {
        ON = 40,
        OFF = 32,
    };

 private:
    /** ------------------------------------------------------------------------------------------------------------
     *  ISO 23092-1 Section 6.5.2 table 19
     *  ------------------------------------------------------------------------------------------------------------ */
    uint8_t group_ID;
    uint16_t ID;
    std::string version;
    bool multiple_alignment_flag;
    ByteOffsetSize byte_offset_size_flag;
    bool non_overlapping_AU_range_flag;
    Pos40Size pos_40_bits_flag;
    bool block_header_flag;

    /// Optional flags
    bool MIT_flag;
    bool CC_mode_flag;
    bool ordered_blocks_flag;

    uint8_t reference_ID;
    std::vector<uint16_t> seq_IDs;
    std::vector<uint16_t> seq_blocks;

    core::parameter::DataUnit::DatasetType dataset_type;

    std::vector<ClassDescription> class_descs; /// num_classes, clid, num_descriptors[], descriptor_ID[][]

    bool parameters_update_flag;
    core::AlphabetID alphabet_ID;

    UAccessUnitInfo u_access_unit_info; /// num_U_access_units, num_U_clusters, multiple_signature_base
                                        /// U_signature_size, U_signature_constant_length, U_signature_length

    std::vector<uint32_t> thress;  ///

 public:
    /**
     * @brief
     */
    DatasetHeader();

//    /**
//     * @brief
//     * @param datasetID
//     */
//    explicit DatasetHeader(uint16_t datasetID);
//
//    /**
//     * @brief
//     * @param group_ID
//     * @param ID
//     * @param _byte_offset_size_flag
//     * @param _non_overlapping_AU_range_flag
//     * @param _pos_40_bits_flag
//     * @param _multiple_alignment_flag
//     * @param _dataset_type
//     * @param _alphabet_ID
//     * @param _num_U_access_units
//     */
//    DatasetHeader(uint8_t group_ID, uint16_t ID, ByteOffsetSize _byte_offset_size_flag,
//                  bool _non_overlapping_AU_range_flag, Pos40Size _pos_40_bits_flag, bool _multiple_alignment_flag,
//                  core::parameter::DataUnit::DatasetType _dataset_type, uint8_t _alphabet_ID,
//                  uint32_t _num_U_access_units);

    /**
     * @brief
     * @param reader
     * @param length
     */
    DatasetHeader(genie::util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length);

    /**
     * @brief
     * @return
     */
    uint8_t getGroupID() const;

    /**
     * @brief
     * @param group_ID
     */
    void setGroupId(uint8_t _group_ID);

    /**
     * @brief
     * @return
     */
    uint16_t getID() const;

    /**
     * @brief
     * @param ID
     */
    void setID(uint16_t _ID);

    /**
     * @brief
     * @return
     */
    ByteOffsetSize getByteOffsetSizeFlag() const;

    /**
     *
     * @return
     */
    bool getNonOverlappingAURangeFlag() const;

    /**
     * @brief
     * @return
     */
    Pos40Size getPos40SizeFlag() const;

    /**
     *
     * @return
     */
    bool getBlockHeaderFlag() const;

    /**
     *
     * @return
     */
    bool getMITFlag() const;

    /**
     *
     * @return
     */
    bool getCCModeFlag() const;

    /**
     *
     * @return
     */
    bool getOrderedBlocksFlag() const;

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
     *
     * @return
     */
    bool getParamUpdateFlag() const;

    /**
     *
     * @return
     */
    core::AlphabetID getAlphabetID() const;

    /**
     *
     * @return
     */
    const UAccessUnitInfo& getUAccessUnitInfo() const;

    /**
     *
     * @return
     */
    uint32_t getNumAccessUnits() const;

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

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
