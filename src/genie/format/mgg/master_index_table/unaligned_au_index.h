/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_UNALIGNED_AU_INDEX_H_
#define SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_UNALIGNED_AU_INDEX_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <optional>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/format/mgb/signature_cfg.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::master_index_table {

/**
 * @brief
 */
class UnalignedAUIndex {
 private:
    uint64_t au_byte_offset;                                  //!< @brief
    std::optional<genie::format::mgb::RefCfg> ref_cfg;        //!< @brief
    std::optional<genie::format::mgb::SignatureCfg> sig_cfg;  //!< @brief
    std::vector<uint64_t> block_byte_offset;                  //!< @brief

    uint8_t byte_offset_size;  //!< @brief
    uint8_t position_size;     //!< @brief
    uint8_t signature_size;    //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const UnalignedAUIndex& other) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<uint64_t>& getBlockOffsets() const;

    /**
     * @brief
     * @param reader
     * @param _byte_offset_size
     * @param _position_size
     * @param dataset_type
     * @param signature_flag
     * @param signature_const_flag
     * @param _signature_size
     * @param block_header_flag
     * @param descriptors
     * @param alphabet
     */
    explicit UnalignedAUIndex(util::BitReader& reader, uint8_t _byte_offset_size, uint8_t _position_size,
                              core::parameter::DataUnit::DatasetType dataset_type, bool signature_flag,
                              bool signature_const_flag, uint8_t _signature_size, bool block_header_flag,
                              const std::vector<genie::core::GenDesc>& descriptors, core::AlphabetID alphabet);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint64_t getAUOffset() const;

    /**
     * @brief
     * @param offset
     */
    void addBlockOffset(uint64_t offset);

    /**
     * @brief
     * @param _au_byte_offset
     * @param _byte_offset_size
     * @param _position_size
     * @param _signature_size
     */
    UnalignedAUIndex(uint64_t _au_byte_offset, uint8_t _byte_offset_size, int8_t _position_size,
                     int8_t _signature_size);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool isReference() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const genie::format::mgb::RefCfg& getReferenceInfo() const;

    /**
     * @brief
     * @param _ref_cfg
     */
    void setReferenceInfo(const genie::format::mgb::RefCfg& _ref_cfg);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool hasSignature() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const genie::format::mgb::SignatureCfg& getSignatureInfo() const;

    /**
     * @brief
     * @param sigcfg
     */
    void setSignatureInfo(const genie::format::mgb::SignatureCfg& sigcfg);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::master_index_table

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_MASTER_INDEX_TABLE_UNALIGNED_AU_INDEX_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
