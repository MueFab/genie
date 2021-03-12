/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DATASET_HEADER_H
#define GENIE_DATASET_HEADER_H

class DatasetType;
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>

#include "fields/block_header_flags/block_header_flags.h"
#include "fields/seq_info/seq_info.h"
#include "fields/u_access_unit_info/u_access_unit_info.h"
#include <parameter/data_unit.h>

namespace genie {
namespace format {
namespace mpegg_p1 {

class DatasetHeader {
   public:
    enum class ByteOffsetSizeFlag: uint8_t {
        ON = 64,
        OFF = 32,
    };

    enum class Pos40SizeFlag: uint8_t {
        ON = 40,
        OFF = 32,
    };
   private:
    /** ------------------------------------------------------------------------------------------------------------
     *  ISO 23092-1 Section 6.5.2 table 19
     *  ------------------------------------------------------------------------------------------------------------ */
    uint8_t dataset_group_ID;
    uint16_t dataset_ID;
    std::string version;
    ByteOffsetSizeFlag byte_offset_size_flag;
    bool non_overlapping_AU_range_flag;
    Pos40SizeFlag pos_40_bits_flag;
    bool multiple_alignment_flag;       // TODO: check existence of variable

    /// block_header_flag, MIT_flag, CC_mode_flag, ordered_blocks_flag
    /// ClassInfo (num_classes, clid[], num_descriptors[], descriptor_ID[][])
    BlockConfig block_header;

    /// seq_count, reference_ID, seq_ID[], seq_blocks[], tflag[], thres[]
    SequenceConfig seq_info;

    core::parameter::DataUnit::DatasetType dataset_type;

    uint8_t alphabet_ID;
    uint32_t num_U_access_units;

    ///num_U_clusters, multiple_signature_base, U_signature_size, U_signature_constant_length, U_signature_length
    std::unique_ptr<UAccessUnitInfo> u_access_unit_info;

   public:
    /**
     *
     */
    DatasetHeader();

    /**
     *
     * @param datasetID
     */
    explicit DatasetHeader(uint16_t datasetID);

    /**
     *
     * @param group_ID
     * @param ID
     * @param _byte_offset_size_flag
     * @param _non_overlapping_AU_range_flag
     * @param _pos_40_bits_flag
     * @param _dataset_type
     * @param _alphabet_ID
     * @param _num_U_access_units
     */
    DatasetHeader(uint8_t group_ID, uint16_t ID, ByteOffsetSizeFlag _byte_offset_size_flag,
                  bool _non_overlapping_AU_range_flag, Pos40SizeFlag _pos_40_bits_flag,
                  bool _multiple_alignment_flag, core::parameter::DataUnit::DatasetType _dataset_type,
                  uint8_t _alphabet_ID, uint32_t _num_U_access_units);

    /**
     *
     * @param bit_reader
     * @param length
     */
    DatasetHeader(genie::util::BitReader& bit_reader, size_t length);

    /**
     *
     * @return
     */
    uint16_t getID() const;
    /**
     *
     * @param ID
     */
    void setID(uint16_t ID);
    /**
     *
     * @return
     */
    uint8_t getGroupID() const;
    /**
     *
     * @param group_ID
     */
    void setGroupId(uint8_t group_ID);


    /**
     *
     * @return
     */
    ByteOffsetSizeFlag getByteOffsetSizeFlag() const;
    /**
     *
     * @return
     */
    Pos40SizeFlag getPos40SizeFlag() const;
    /**
     *
     * @return
     */
    const SequenceConfig& getSeqInfo() const;
    /**
     *
     * @return
     */
    const BlockConfig& getBlockHeader() const;
    /**
     *
     * @return
     */
    uint32_t getNumUAccessUnits() const;
    /**
     *
     * @return
     */
    bool getMultipleAlignmentFlag() const;
    /**
     *
     * @return
     */
    core::parameter::DataUnit::DatasetType getDatasetType() const;


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

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_DATASET_HEADER_H
