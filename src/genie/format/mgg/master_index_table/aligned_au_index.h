/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_ALIGNED_AU_INDEX_H_
#define SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_ALIGNED_AU_INDEX_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include <optional>
#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/format/mgb/extended_au.h"
#include "genie/format/mgb/ref_cfg.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace master_index_table {

/**
 * @brief
 */
class AlignedAUIndex {
 private:
    uint64_t au_byte_offset;                                       //!< @brief
    uint64_t au_start_position;                                    //!< @brief
    uint64_t au_end_position;                                      //!< @brief
    std::optional<genie::format::mgb::RefCfg> ref_cfg;           //!< @brief
    std::optional<genie::format::mgb::ExtendedAu> extended_cfg;  //!< @brief
    std::vector<uint64_t> block_byte_offset;                       //!< @brief

    uint8_t byte_offset_size;  //!< @brief
    uint8_t position_size;     //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const AlignedAUIndex& other) const;

    /**
     * @brief
     * @param _au_byte_offset
     * @param _au_start_position
     * @param _au_end_position
     * @param _byte_offset_size
     * @param _position_size
     */
    AlignedAUIndex(uint64_t _au_byte_offset, uint64_t _au_start_position, uint64_t _au_end_position,
                   uint8_t _byte_offset_size, uint8_t _position_size);

    /**
     * @brief
     * @param reader
     * @param _byte_offset_size
     * @param _position_size
     * @param dataset_type
     * @param multiple_alignment
     * @param block_header_flag
     * @param descriptors
     */
    AlignedAUIndex(genie::util::BitReader& reader, uint8_t _byte_offset_size, uint8_t _position_size,
                   core::parameter::DataUnit::DatasetType dataset_type, bool multiple_alignment, bool block_header_flag,
                   const std::vector<genie::core::GenDesc>& descriptors);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    uint64_t getByteOffset() const;

    /**
     * @brief
     * @return
     */
    uint64_t getAUStartPos() const;

    /**
     * @brief
     * @return
     */
    uint64_t getAUEndPos() const;

    /**
     * @brief
     * @return
     */
    bool isReference() const;

    /**
     * @brief
     * @return
     */
    const genie::format::mgb::RefCfg& getReferenceInfo() const;

    /**
     * @brief
     * @param _ref_cfg
     */
    void setReferenceInfo(genie::format::mgb::RefCfg _ref_cfg);

    /**
     * @brief
     * @param _ext_au
     */
    void setExtended(genie::format::mgb::ExtendedAu _ext_au);

    /**
     * @brief
     * @return
     */
    const genie::format::mgb::ExtendedAu& getExtension() const;

    /**
     * @brief
     * @return
     */
    bool isExtended() const;

    /**
     * @brief
     * @return
     */
    const std::vector<uint64_t>& getBlockOffsets() const;

    /**
     * @brief
     * @param offset
     */
    void addBlockOffset(uint64_t offset);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace master_index_table
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_ALIGNED_AU_INDEX_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
