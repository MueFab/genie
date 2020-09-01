/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_PART1_DATASET_H
#define GENIE_PART1_DATASET_H

#include <memory>
#include <vector>

#include <genie/core/parameter/data_unit.h>
#include <genie/core/parameter/parameter_set.h>
#include <genie/format/mgb/data-unit-factory.h>

#include <genie/format/mpegg_p1/dataset/master_index_table/master_index_table.h>
#include <genie/format/mpegg_p1/dataset/access_unit/access_unit.h>
#include <genie/format/mpegg_p1/dataset/dataset_parameter_set.h>

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 *
 */
class DTMetadata {
   public:
    /**
     *
     */
    DTMetadata();

    uint64_t getLength() const;

    void write(genie::util::BitWriter& bit_writer) const;

   private:
    std::vector<uint8_t> DT_metadata_value;  //!<
};

/**
 *
 */
class DTProtection {
   public:
    /**
     *
     */
    DTProtection();

    uint64_t getLength() const;

    void write(genie::util::BitWriter& bit_writer) const;

   private:
    std::vector<uint8_t> DT_protection_value;  //!<
};

class Dataset {
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

    // block_header_flag, MIT_flag, CC_mode_flag, ordered_blocks_flag, num_classes, clid[],
    // num_descriptors[], descriptor_ID[][]
    BlockConfig block_config;

    core::parameter::DataUnit::DatasetType dataset_type;

    uint8_t alphabet_ID;
    uint32_t num_U_access_units;

    // num_U_clusters, multiple_signature_base, U_signature_size, U_signature_constant_length, U_signature_length
    std::unique_ptr<UAccessUnitInfo> u_access_unit_info;

    // seq_count, reference_ID, seq_ID[], seq_blocks[], tflag[], thres[]
    SequenceConfig seq_info;

    /** ------------------------------------------------------------------------------------------------------------
     *  ISO 23092-1 Section 6.5.2.2 table 18
     *  ------------------------------------------------------------------------------------------------------------ */

//    DatasetHeader dataset_header;

    // ISO 23092-1 Section 6.5.2.3 - specification 23092-3
    // optional
    std::unique_ptr<DTMetadata> DT_metadata;

    // ISO 23092-1 Section 6.5.2.4 - specification 23092-3
    // optional
    std::unique_ptr<DTProtection> DT_protection;

    // ISO 23092-1 Section 6.5.2.5 - specification 23092-2
    // optional
    std::vector<DatasetParameterSet> dataset_parameter_sets;

    // ISO 23092-1 Section 6.5.3.1 - specification 23092-1
    // optional
    std::unique_ptr<MasterIndexTable> master_index_table;

    // ISO 23092-1 Section 6.5.3
    std::vector<AccessUnit> access_units;

    // TODO(Yeremia): Descriptor Stream
    // ISO 23092-1 Section 6.5.4
//    std::vector<DescriptorStream> descriptor_streams;

   public:

    Dataset(uint16_t _dataset_ID);

    /**
     *
     * @param dataUnitFactory
     * @param accessUnits_p2
     * @param ID
     */
    Dataset(uint16_t ID, const genie::format::mgb::DataUnitFactory& dataUnitFactory,
            std::vector<genie::format::mgb::AccessUnit>& accessUnits_p2);

    /**
     *
     * @param _dataset_group_ID
     */
    void setGroupId(uint8_t group_ID);

    /**
     *
     * @return
     */
    uint8_t getGroupID() const;

    /**
     *
     * @param ID
     */
    void setID(uint16_t ID);

    /**
     *
     * @return
     */
    uint16_t getID() const;

    /**
     *
     * @return
     */
    const std::vector<DatasetParameterSet>& getParameterSets() const;

    /**
     *
     * @return length of dataset_header in bytes
     */
    uint64_t getHeaderLength() const;

    /**
     *
     * @param bit_writer
     */
    void writeHeader(util::BitWriter& bit_writer) const;

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

#endif  // GENIE_PART1_DATASET_H
