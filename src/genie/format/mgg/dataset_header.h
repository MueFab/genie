/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_HEADER_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "boost/optional/optional.hpp"
#include "genie/core/constants.h"
#include "genie/core/meta/block_header.h"
#include "genie/core/meta/block_header/disabled.h"
#include "genie/core/meta/block_header/enabled.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/core/record/class_type.h"
#include "genie/format/mgg/dataset_header/block_header_off_options.h"
#include "genie/format/mgg/dataset_header/block_header_on_options.h"
#include "genie/format/mgg/dataset_header/mit_class_config.h"
#include "genie/format/mgg/dataset_header/reference_options.h"
#include "genie/format/mgg/dataset_header/u_options.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit_reader.h"
#include "genie/util/make_unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

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

    boost::optional<dataset_header::BlockHeaderOnOptions> block_header_on;    //!< @brief
    boost::optional<dataset_header::BlockHeaderOffOptions> block_header_off;  //!< @brief

    dataset_header::ReferenceOptions referenceOptions;  //!< @brief

    core::parameter::DataUnit::DatasetType dataset_type;  //!< @brief

    std::vector<dataset_header::MITClassConfig> mit_configs;  //!< @brief
    bool parameters_update_flag;                              //!< @brief
    core::AlphabetID alphabet_id;                             //!< @brief

    uint32_t num_U_access_units;                          //!< @brief
    boost::optional<dataset_header::UOptions> u_options;  //!< @brief

    std::vector<boost::optional<uint32_t>> thresholds;  //!< @brief

 public:
    /**
     * @brief
     * @param _old
     * @param _new
     */
    void patchRefID(uint8_t _old, uint8_t _new);

    /**
     * @brief
     * @param _groupID
     * @param _setID
     */
    void patchID(uint8_t _groupID, uint16_t _setID);

    /**
     * @brief
     * @return
     */
    std::unique_ptr<core::meta::BlockHeader> decapsulate();

    /**
     * @brief
     * @param output
     * @param depth
     * @param max_depth
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;

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
    const dataset_header::ReferenceOptions& getReferenceOptions() const;

    /**
     * @brief
     * @return
     */
    core::parameter::DataUnit::DatasetType getDatasetType() const;

    /**
     * @brief
     * @return
     */
    const std::vector<dataset_header::MITClassConfig>& getMITConfigs() const;

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
    const dataset_header::UOptions& getUOptions() const;

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
    void setUAUs(uint32_t _num_U_access_units, dataset_header::UOptions u_opts);

    /**
     * @brief
     * @param config
     */
    void addClassConfig(dataset_header::MITClassConfig config);

    /**
     * @brief
     * @param opts
     */
    void disableBlockHeader(dataset_header::BlockHeaderOffOptions opts);

    /**
     * @brief
     */
    void disableMIT();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
