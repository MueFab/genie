/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_HEADER_H_
#define SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <map>
#include <memory>
#include <vector>
#include "genie/core/parameter/data_unit.h"
#include "genie/core/record/record.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/entropy/gabac/gabac.h"
#include "genie/format/mgb/au_type_cfg.h"
#include "genie/format/mgb/block.h"
#include "genie/format/mgb/mm_cfg.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/format/mgb/signature_cfg.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 * @brief
 */
class AUHeader {
 private:
    uint32_t access_unit_ID;                         //!< @brief
    uint8_t num_blocks;                              //!< @brief
    uint8_t parameter_set_ID;                        //!< @brief
    core::record::ClassType au_type;                 //!< @brief
    uint32_t reads_count;                            //!< @brief
    std::optional<MmCfg> mm_cfg;                   //!< @brief
    std::optional<RefCfg> ref_cfg;                 //!< @brief
    std::optional<AuTypeCfg> au_Type_U_Cfg;        //!< @brief
    std::optional<SignatureCfg> signature_config;  //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const AUHeader &other) const;

    /**
     * @brief
     * @param writer
     * @param write_signatures
     */
    void write(genie::util::BitWriter &writer, bool write_signatures) const;

    /**
     * @brief
     * @return
     */
    uint32_t getNumBlocks() const;

    /**
     * @brief
     */
    void blockAdded();

    /**
     * @brief
     * @param bitReader
     * @param parameterSets
     * @param read_signatures
     */
    AUHeader(util::BitReader &bitReader, const std::map<size_t, core::parameter::EncodingSet> &parameterSets,
             bool read_signatures = true);

    /**
     * @brief
     */
    AUHeader();

    /**
     * @brief
     * @param _access_unit_ID
     * @param _parameter_set_ID
     * @param _au_type
     * @param _reads_count
     * @param dataset_type
     * @param posSize
     * @param signatureFlag
     * @param alphabet
     */
    AUHeader(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, core::record::ClassType _au_type,
             uint32_t _reads_count, genie::core::parameter::ParameterSet::DatasetType dataset_type, uint8_t posSize,
             bool signatureFlag, core::AlphabetID alphabet);

    /**
     * @brief
     * @return
     */
    uint32_t getID() const;

    /**
     * @brief
     * @return
     */
    uint8_t getParameterID() const;

    /**
     * @brief
     * @return
     */
    const AuTypeCfg &getAlignmentInfo() const;

    /**
     * @brief
     * @return
     */
    uint32_t getReadCount() const;

    /**
     * @brief
     * @return
     */
    core::record::ClassType getClass() const;

    /**
     * @brief
     * @param cfg
     */
    void setMmCfg(MmCfg &&cfg);

    /**
     * @brief
     * @param cfg
     */
    void setRefCfg(RefCfg &&cfg);

    /**
     * @brief
     * @return
     */
    const RefCfg &getRefCfg();

    /**
     * @brief
     * @param cfg
     */
    void setAuTypeCfg(AuTypeCfg &&cfg);

    /**
     * @brief
     * @param cfg
     */
    void setSignatureCfg(SignatureCfg &&cfg);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
