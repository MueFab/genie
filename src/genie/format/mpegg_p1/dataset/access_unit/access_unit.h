/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <vector>
#include "genie/core/parameter/data_unit.h"
#include "genie/core/record/class-type.h"
#include "genie/format/mgb/access_unit.h"
#include "genie/format/mgb/au_type_cfg.h"
#include "genie/format/mgb/mm_cfg.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/format/mgb/signature_cfg.h"
#include "genie/format/mpegg_p1/dataset/access_unit/block.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class AUInformation {
 private:
    std::vector<uint8_t> AU_information_value;  //!< @brief

 public:
    /**
     * @brief
     */
    AUInformation();

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

/**
 * @brief
 */
class AUProtection {
 private:
    std::vector<uint8_t> AU_protection_value;  //!< @brief

 public:
    /**
     * @brief
     */
    AUProtection();

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

/**
 * @brief
 */
class AccessUnit : public core::parameter::DataUnit {
 private:
    /* ----- internal ----- */
    bool block_header_flag;  //!< @brief From Dataset/BlockHeaderFlags/block_header_flag
    // already inside ref_cfg
    // uint8_t posSize;
    // already inside signature_config
    // uint8_t U_signature_size;

    /* ----- access_unit_header ----- */
    uint32_t access_unit_ID;          //!< @brief
    uint8_t num_blocks;               //!< @brief
    uint8_t parameter_set_ID;         //!< @brief
    core::record::ClassType au_type;  //!< @brief
    uint32_t reads_count;             //!< @brief

    std::unique_ptr<mgb::MmCfg> mm_cfg;                   //!< @brief
    std::unique_ptr<mgb::RefCfg> ref_cfg;                 //!< @brief
    std::unique_ptr<mgb::AuTypeCfg> au_Type_U_Cfg;        //!< @brief
    std::unique_ptr<mgb::SignatureCfg> signature_config;  //!< @brief

    /* ----- access_unit ----- */

    std::unique_ptr<AUInformation> AU_information;  //!< @brief
    std::unique_ptr<AUProtection> AU_protection;    //!< @brief

    std::list<Block> blocks;  //!< @brief

 public:
    /**
     * @brief
     * @param _access_unit_ID
     * @param _parameter_set_ID
     * @param _au_type
     * @param _reads_count
     * @param dataset_type
     * @param posSize
     * @param signatureSize
     * @param multiple_signature_base
     */
    AccessUnit(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, core::record::ClassType _au_type,
               uint32_t _reads_count, DatasetType dataset_type, uint8_t posSize, uint8_t signatureSize,
               uint32_t multiple_signature_base);

    /**
     * @brief
     * @param au
     */
    explicit AccessUnit(format::mgb::AccessUnit&& au);

    /**
     * @brief
     * @return
     */
    uint64_t getHeaderLength() const;

    /**
     * @brief
     * @param writer
     * @param write_dummy_length
     */
    void writeHeader(util::BitWriter& writer, bool write_dummy_length = false) const;

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
