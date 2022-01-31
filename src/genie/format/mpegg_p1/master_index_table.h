/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MASTER_INDEX_TABLE_H
#define GENIE_MASTER_INDEX_TABLE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/format/mgb/extended_au.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/format/mgb/signature_cfg.h"
#include "genie/format/mpegg_p1/dataset_header.h"
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/util/bitreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class AlignedAUIndex {
 private:
    uint64_t au_byte_offset;                                       //!< @brief
    uint64_t au_start_position;                                    //!< @brief
    uint64_t au_end_position;                                      //!< @brief
    boost::optional<genie::format::mgb::RefCfg> ref_cfg;           //!< @brief
    boost::optional<genie::format::mgb::ExtendedAu> extended_cfg;  //!< @brief
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

/**
 * @brief
 */
class UnalignedAUIndex {
 private:
    uint64_t au_byte_offset;                                    //!< @brief
    boost::optional<genie::format::mgb::RefCfg> ref_cfg;        //!< @brief
    boost::optional<genie::format::mgb::SignatureCfg> sig_cfg;  //!< @brief
    std::vector<uint64_t> block_byte_offset;                    //!< @brief

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
    const std::vector<uint64_t>& getBlockOffsets() const;

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
     */
    explicit UnalignedAUIndex(util::BitReader& reader, uint8_t _byte_offset_size, uint8_t _position_size,
                              core::parameter::DataUnit::DatasetType dataset_type, bool signature_flag,
                              bool signature_const_flag, uint8_t _signature_size, bool block_header_flag,
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
    uint64_t getAUOffset() const;

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
     * @return
     */
    bool hasSignature() const;

    /**
     * @brief
     * @return
     */
    const genie::format::mgb::SignatureCfg& getSignatureInfo() const;

    /**
     * @brief
     * @param sigcfg
     */
    void setSignatureInfo(genie::format::mgb::SignatureCfg sigcfg);
};

/**
 * @brief
 */
class MasterIndexTable : public GenInfo {
 private:
    std::vector<std::vector<std::vector<AlignedAUIndex>>> aligned_aus;  //!< @brief
    std::vector<UnalignedAUIndex> unaligned_aus;                        //!< @brief

 public:
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
    const std::vector<std::vector<std::vector<AlignedAUIndex>>>& getAlignedAUs() const;

    /**
     * @brief
     * @return
     */
    const std::vector<UnalignedAUIndex>& getUnalignedAUs() const;

    /**
     * @brief
     * @param seq_count
     * @param num_classes
     */
    MasterIndexTable(uint16_t seq_count, uint8_t num_classes);

    /**
     * @brief
     * @param reader
     * @param hdr
     */
    MasterIndexTable(util::BitReader& reader, const DatasetHeader& hdr);

    /**
     * @brief
     * @param bitWriter
     */
    void write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief
     * @return
     */
    uint64_t getSize() const override;

    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MASTER_INDEX_TABLE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
