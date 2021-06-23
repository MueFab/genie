/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include "genie/core/parameter/data_unit.h"
#include "genie/core/parameter/parameter_set.h"
#include "genie/format/mgb/data-unit-factory.h"
#include "genie/format/mpegg_p1/dataset/access_unit/access_unit.h"
#include "genie/format/mpegg_p1/dataset/dataset_header.h"
#include "genie/format/mpegg_p1/dataset/dataset_parameter_set.h"
#include "genie/format/mpegg_p1/dataset/descriptor_stream.h"
#include "genie/format/mpegg_p1/dataset/master_index_table/master_index_table.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DTMetadata {
 public:
    /**
     * @brief
     */
    DTMetadata();

    /**
     * @brief
     * @param _DT_metadata_value
     */
    explicit DTMetadata(std::vector<uint8_t>&& _DT_metadata_value);

    /**
     * @brief
     * @param reader
     * @param length
     */
    DTMetadata(genie::util::BitReader& reader, size_t length);

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

 private:
    std::vector<uint8_t> DT_metadata_value;  //!< @brief
};

/**
 * @brief
 */
class DTProtection {
 public:
    /**
     * @brief
     */
    DTProtection();

    /**
     * @brief
     * @param _DT_metadata_value
     */
    explicit DTProtection(std::vector<uint8_t>&& _DT_protection_value);

    /**
     * @brief
     * @param reader
     * @param length
     */
    DTProtection(genie::util::BitReader& reader, size_t length);

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

 private:
    std::vector<uint8_t> DT_protection_value;  //!< @brief
};

/**
 * @brief
 */
class Dataset {
 private:
    /** ------------------------------------------------------------------------------------------------------------
     *  ISO 23092-1 Section 6.5.2.2 table 18
     *  ------------------------------------------------------------------------------------------------------------ */

    DatasetHeader header;  //!< @brief

    std::unique_ptr<DTMetadata> DT_metadata;  //!< @brief ISO 23092-1 Section 6.5.2.3 - specification 23092-3, optional

    std::unique_ptr<DTProtection>
        DT_protection;  //!< @brief ISO 23092-1 Section 6.5.2.4 - specification 23092-3, optional

    std::vector<DatasetParameterSet>
        dataset_parameter_sets;  //!< @brief ISO 23092-1 Section 6.5.2.5 - specification 23092-2, optional

    std::vector<AccessUnit> access_units;  //!< @brief ISO 23092-1 Section 6.5.3

    std::vector<DescriptorStream> descriptor_streams;  //!< @brief ISO 23092-1 Section 6.5.4

 public:
    /**
     * @brief
     * @param _dataset_ID
     */
    explicit Dataset(uint16_t _dataset_ID);

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
     * @param accessUnits_p2
     * @param dataUnitFactory
     */
    Dataset(uint8_t group_ID, uint16_t ID, DatasetHeader::ByteOffsetSizeFlag _byte_offset_size_flag,
            bool _non_overlapping_AU_range_flag, DatasetHeader::Pos40SizeFlag _pos_40_bits_flag,
            bool _multiple_alignment_flag, core::parameter::DataUnit::DatasetType _dataset_type, uint8_t _alphabet_ID,
            uint32_t _num_U_access_units, std::vector<genie::format::mgb::AccessUnit>& accessUnits_p2,
            const genie::format::mgb::DataUnitFactory& dataUnitFactory);

    /**
     * @brief
     * @param reader
     * @param length
     */
    Dataset(util::BitReader& reader, size_t length);

    /**
     * @brief
     * @return
     */
    const std::vector<DatasetParameterSet>& getParameterSets() const;

    /**
     * @brief
     * @return
     */
    const DatasetHeader& getHeader() const;

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
    DatasetHeader::ByteOffsetSizeFlag getByteOffsetSizeFlag() const;

    /**
     * @brief
     * @return
     */
    DatasetHeader::Pos40SizeFlag getPos40SizeFlag() const;

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

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
