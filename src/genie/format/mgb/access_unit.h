/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ACCESS_UNIT_H
#define GENIE_ACCESS_UNIT_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/parameter/data_unit.h>
#include <genie/core/record/record.h>
#include <genie/core/stats/perf-stats.h>
#include <genie/entropy/gabac/gabac.h>
#include <genie/format/mgb/au_type_cfg.h>
#include <genie/format/mgb/block.h>
#include <genie/format/mgb/mm_cfg.h>
#include <genie/format/mgb/ref_cfg.h>
#include <genie/format/mgb/signature_cfg.h>
#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>
#include <cstdint>
#include <map>
#include <memory>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 *
 */
class AccessUnit : public core::parameter::DataUnit {
   public:
    /**
     *
     * @param parameterSets
     * @param bitReader
     */
    explicit AccessUnit(const std::map<size_t, core::parameter::ParameterSet> &parameterSets,
                        util::BitReader &bitReader, bool lazyPayload = false);

    /**
     *
     * @param bitReader
     */
    void loadPayload(util::BitReader &bitReader);

    /**
     *
     * @return
     */
    size_t getPayloadSize() const;

    /**
     *
     * @param writer
     */
    void write(util::BitWriter &writer) const override;

    /**
     *
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
     *
     * @param block
     */
    void addBlock(Block block);

    /**
     *
     * @param cfg
     */
    void setMmCfg(MmCfg &&cfg);

    /**
     *
     * @param cfg
     */
    void setRefCfg(RefCfg &&cfg);

    /**
     *
     * @return
     */
    const RefCfg &getRefCfg();

    /**
     *
     * @param cfg
     */
    void setAuTypeCfg(AuTypeCfg &&cfg);

    /**
     *
     * @param cfg
     */
    void setSignatureCfg(SignatureCfg &&cfg);

    /**
     *
     * @return
     */
    std::vector<Block> getBlocks() const;

    /**
     *
     * @return
     */
    uint32_t getID() const;

    /**
     *
     * @return
     */
    uint8_t getParameterID() const;

    /**
     *
     * @return
     */
    const AuTypeCfg &getAlignmentInfo() const;

    /**
     *
     * @return
     */
    uint32_t getReadCount() const;

    /**
     *
     * @return
     */
    core::record::ClassType getClass() const;

   private:
    /**
     *
     * @param writer
     */
    void preWrite(util::BitWriter &writer) const;

    uint32_t access_unit_ID;                         //!<
    uint8_t num_blocks;                              //!<
    uint8_t parameter_set_ID;                        //!<
    core::record::ClassType au_type;                 //!<
    uint32_t reads_count;                            //!<
    boost::optional<MmCfg> mm_cfg;                   //!<
    boost::optional<RefCfg> ref_cfg;                 //!<
    boost::optional<AuTypeCfg> au_Type_U_Cfg;        //!<
    boost::optional<SignatureCfg> signature_config;  //!<

    std::vector<Block> blocks;  //!<

    size_t payloadbytes;  //!<
    size_t qv_payloads;   //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ACCESS_UNIT_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
