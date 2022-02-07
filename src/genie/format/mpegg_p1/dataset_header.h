/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <memory>
#include <vector>
#include "boost/optional/optional.hpp"
#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/core/record/class-type.h"
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/util/bitreader.h"
#include "genie/util/make-unique.h"
#include "genie/core/meta/blockheader.h"
#include "genie/core/meta/blockheader/enabled.h"
#include "genie/core/meta/blockheader/disabled.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class BlockHeaderOnOptions {
 private:
    bool mit_flag;      //!< @brief
    bool cc_mode_flag;  //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const BlockHeaderOnOptions& other) const;

    /**
     * @brief
     * @param _mit_flag
     * @param _cc_mode_flag
     */
    BlockHeaderOnOptions(bool _mit_flag, bool _cc_mode_flag);

    /**
     * @brief
     * @param reader
     */
    explicit BlockHeaderOnOptions(genie::util::BitReader& reader);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    bool getMITFlag() const;

    /**
     * @brief
     * @return
     */
    bool getCCFlag() const;
};

/**
 * @brief
 */
struct BlockHeaderOffOptions {
 private:
    bool ordered_blocks_flag;  //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const BlockHeaderOffOptions& other) const;

    /**
     * @brief
     * @param _ordered_blocks_flag
     */
    explicit BlockHeaderOffOptions(bool _ordered_blocks_flag);

    /**
     * @brief
     * @param reader
     */
    explicit BlockHeaderOffOptions(util::BitReader& reader);

    /**
     * @brief
     * @return
     */
    bool getOrderedBlocksFlag() const;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;
};

/**
 * @brief
 */
class ReferenceOptions {
 private:
    uint8_t reference_ID;              //!< @brief
    std::vector<uint16_t> seq_ID;      //!< @brief
    std::vector<uint32_t> seq_blocks;  //!< @brief

 public:

    void patchRefID(uint8_t _old, uint8_t _new) {
        if(reference_ID == _old) {
            reference_ID = _new;
        }
    }

    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const ReferenceOptions& other) const;

    /**
     * @brief
     * @param reader
     */
    explicit ReferenceOptions(genie::util::BitReader& reader);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief
     */
    ReferenceOptions();

    /**
     * @brief
     * @param _reference_ID
     * @param _seq_id
     * @param blocks
     */
    void addSeq(uint8_t _reference_ID, uint8_t _seq_id, uint16_t blocks);

    /**
     * @brief
     * @return
     */
    const std::vector<uint16_t>& getSeqIDs() const;

    /**
     * @brief
     * @return
     */
    const std::vector<uint32_t>& getSeqBlocks() const;

    /**
     * @brief
     * @return
     */
    uint8_t getReferenceID() const;
};

/**
 * @brief
 */
class MITClassConfig {
 private:
    genie::core::record::ClassType id;                 //!< @brief
    std::vector<genie::core::GenDesc> descriptor_ids;  //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const MITClassConfig& other) const;

    /**
     * @brief
     * @param _id
     */
    explicit MITClassConfig(genie::core::record::ClassType _id);

    /**
     * @brief
     * @param reader
     * @param block_header_flag
     */
    MITClassConfig(genie::util::BitReader& reader, bool block_header_flag);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief
     * @param desc
     */
    void addDescriptorID(genie::core::GenDesc desc);

    /**
     * @brief
     * @return
     */
    genie::core::record::ClassType getClassID() const;

    /**
     * @brief
     * @return
     */
    const std::vector<genie::core::GenDesc>& getDescriptorIDs() const;
};

/**
 * @brief
 */
class USignature {
 private:
    boost::optional<uint8_t> const_length;  //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const USignature& other) const;

    /**
     * @brief
     */
    USignature();

    /**
     * @brief
     * @param _const_length
     */
    explicit USignature(uint8_t _const_length);

    /**
     * @brief
     * @param reader
     */
    explicit USignature(genie::util::BitReader& reader);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    bool isConstLength() const;

    /**
     * @brief
     * @return
     */
    uint8_t getConstLength() const;
};

/**
 * @brief
 */
class UOptions {
 private:
    uint64_t reserved1;                       //!< @brief
    boost::optional<USignature> u_signature;  //!< @brief
    boost::optional<uint8_t> reserved2;       //!< @brief
    bool reserved3;                           //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const UOptions& other) const;

    /**
     * @brief
     * @param _reserved1
     * @param _reserved3
     */
    explicit UOptions(uint64_t _reserved1 = 0, bool _reserved3 = false);

    /**
     * @brief
     * @param reader
     */
    explicit UOptions(genie::util::BitReader& reader);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    uint64_t getReserved1() const;

    /**
     * @brief
     * @return
     */
    bool getReserved3() const;

    /**
     * @brief
     * @return
     */
    bool hasReserved2() const;

    /**
     * @brief
     * @return
     */
    uint8_t getReserved2() const;

    /**
     * @brief
     * @return
     */
    bool hasSignature() const;

    /**
     * @brief
     * @return
     */
    const USignature& getSignature() const;

    /**
     * @brief
     * @param s
     */
    void addSignature(USignature s);

    /**
     * @brief
     * @param r
     */
    void addReserved2(uint8_t r);
};

/**
 * @brief
 */
class DatasetHeader : public GenInfo {
 private:
    uint8_t group_ID;                       //!< @brief
    uint16_t ID;                            //!< @brief
    genie::core::MPEGMinorVersion version;  //!< @brief
    bool multiple_alignment_flag;           //!< @brief
    bool byte_offset_size_flag;             //!< @brief
    bool non_overlapping_AU_range_flag;     //!< @brief
    bool pos_40_bits_flag;                  //!< @brief

    boost::optional<BlockHeaderOnOptions> block_header_on;    //!< @brief
    boost::optional<BlockHeaderOffOptions> block_header_off;  //!< @brief

    ReferenceOptions referenceOptions;  //!< @brief

    core::parameter::DataUnit::DatasetType dataset_type;  //!< @brief

    std::vector<MITClassConfig> mit_configs;  //!< @brief
    bool parameters_update_flag;              //!< @brief
    core::AlphabetID alphabet_id;             //!< @brief

    uint32_t num_U_access_units;          //!< @brief
    boost::optional<UOptions> u_options;  //!< @brief

    std::vector<boost::optional<uint32_t>> thresholds;  //!< @brief

 public:
    void patchRefID(uint8_t _old, uint8_t _new) {
        referenceOptions.patchRefID(_old, _new);
    }

    void patchID(uint8_t _groupID, uint16_t _setID) {
        group_ID = _groupID;
        ID = _setID;
    }

    std::unique_ptr<core::meta::BlockHeader> decapsulate() {
        if (block_header_on != boost::none) {
            return genie::util::make_unique<genie::core::meta::blockheader::Enabled>(block_header_on->getMITFlag(),
                                                                                     block_header_on->getCCFlag());
        } else {
            return genie::util::make_unique<genie::core::meta::blockheader::Disabled>(
                block_header_off->getOrderedBlocksFlag());
        }
    }

    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override {
        print_offset(output, depth, max_depth, "* Dataset Header");
    }

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     * @return
     */
    uint8_t getDatasetGroupID() const;

    /**
     * @brief
     * @return
     */
    uint16_t getDatasetID() const;

    /**
     * @brief
     * @return
     */
    genie::core::MPEGMinorVersion getVersion() const;

    /**
     * @brief
     * @return
     */
    bool getMultipleAlignmentFlag() const;

    /**
     * @brief
     * @return
     */
    uint8_t getByteOffsetSize() const;

    /**
     * @brief
     * @return
     */
    bool getNonOverlappingAURangeFlag() const;

    /**
     * @brief
     * @return
     */
    uint8_t getPosBits() const;

    /**
     * @brief
     * @return
     */
    bool isBlockHeaderEnabled() const;

    /**
     * @brief
     * @return
     */
    bool isMITEnabled() const;

    /**
     * @brief
     * @return
     */
    bool isCCModeEnabled() const;

    /**
     * @brief
     * @return
     */
    bool isOrderedBlockMode() const;

    /**
     * @brief
     * @return
     */
    const ReferenceOptions& getReferenceOptions() const;

    /**
     * @brief
     * @return
     */
    core::parameter::DataUnit::DatasetType getDatasetType() const;

    /**
     * @brief
     * @return
     */
    const std::vector<MITClassConfig>& getMITConfigs() const;

    /**
     * @brief
     * @return
     */
    bool getParameterUpdateFlag() const;

    /**
     * @brief
     * @return
     */
    core::AlphabetID getAlphabetID() const;

    /**
     * @brief
     * @return
     */
    uint32_t getNumUAccessUnits() const;

    /**
     * @brief
     * @return
     */
    const UOptions& getUOptions() const;

    /**
     * @brief
     * @return
     */
    const std::vector<boost::optional<uint32_t>>& getRefSeqThresholds() const;

    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;

    /**
     * @brief
     */
    DatasetHeader();

    /**
     * @brief
     * @param _dataset_group_id
     * @param _dataset_id
     * @param _version
     * @param _multiple_alignments_flags
     * @param _byte_offset_size_flags
     * @param _non_overlapping_AU_range_flag
     * @param _pos_40_bits_flag
     * @param _dataset_type
     * @param _parameters_update_flag
     * @param _alphabet_id
     */
    DatasetHeader(uint8_t _dataset_group_id, uint16_t _dataset_id, genie::core::MPEGMinorVersion _version,
                  bool _multiple_alignments_flags, bool _byte_offset_size_flags, bool _non_overlapping_AU_range_flag,
                  bool _pos_40_bits_flag, core::parameter::DataUnit::DatasetType _dataset_type,
                  bool _parameters_update_flag, core::AlphabetID _alphabet_id);

    /**
     * @brief
     * @param reader
     */
    explicit DatasetHeader(genie::util::BitReader& reader);

    /**
     * @brief
     * @param writer
     */
    void box_write(genie::util::BitWriter& writer) const override;

    /**
     * @brief
     * @param _reference_ID
     * @param _seqID
     * @param _blocks_num
     * @param _threshold
     */
    void addRefSequence(uint8_t _reference_ID, uint16_t _seqID, uint32_t _blocks_num,
                        boost::optional<uint32_t> _threshold);

    /**
     * @brief
     * @param _num_U_access_units
     * @param u_opts
     */
    void setUAUs(uint32_t _num_U_access_units, UOptions u_opts);

    /**
     * @brief
     * @param config
     */
    void addClassConfig(MITClassConfig config);

    /**
     * @brief
     * @param opts
     */
    void disableBlockHeader(BlockHeaderOffOptions opts);

    /**
     * @brief
     */
    void disableMIT();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
