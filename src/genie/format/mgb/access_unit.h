/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_H_
#define SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_H_

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
class AccessUnit : public core::parameter::DataUnit {
 public:
    /**
     * @brief
     */
    void debugPrint(const core::parameter::ParameterSet &ps) const;

    /**
     * @brief
     * @param parameterSets
     * @param bitReader
     * @param lazyPayload
     */
    explicit AccessUnit(const std::map<size_t, core::parameter::ParameterSet> &parameterSets,
                        util::BitReader &bitReader, bool lazyPayload = false);

    /**
     * @brief
     * @param bitReader
     */
    void loadPayload(util::BitReader &bitReader);

    /**
     * @brief
     * @return
     */
    size_t getPayloadSize() const;

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter &writer) const override;

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
     * @param block
     */
    void addBlock(Block block);

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

    /**
     * @brief
     * @return
     */
    std::vector<Block> getBlocks() const;

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

 private:
    /**
     * @brief
     * @param writer
     */
    void preWrite(util::BitWriter &writer) const;

    uint32_t access_unit_ID;                         //!< @brief
    uint8_t num_blocks;                              //!< @brief
    uint8_t parameter_set_ID;                        //!< @brief
    core::record::ClassType au_type;                 //!< @brief
    uint32_t reads_count;                            //!< @brief
    boost::optional<MmCfg> mm_cfg;                   //!< @brief
    boost::optional<RefCfg> ref_cfg;                 //!< @brief
    boost::optional<AuTypeCfg> au_Type_U_Cfg;        //!< @brief
    boost::optional<SignatureCfg> signature_config;  //!< @brief

    std::vector<Block> blocks;  //!< @brief

    size_t payloadbytes;  //!< @brief
    size_t qv_payloads;   //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
