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

class AccessUnit : public core::parameter::DataUnit {
   public:
    explicit AccessUnit(const std::map<size_t, core::parameter::ParameterSet> &parameterSets,
                        util::BitReader &bitReader);

    void write(util::BitWriter &writer, genie::core::stats::PerfStats *stats = nullptr) const override;

    AccessUnit(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, core::record::ClassType _au_type,
               uint32_t _reads_count, DatasetType dataset_type, uint8_t posSize, uint8_t signatureSize,
               uint32_t multiple_signature_base);

    void addBlock(Block block);

    void setMmCfg(MmCfg &&cfg);

    void setRefCfg(RefCfg &&cfg);

    void setAuTypeCfg(AuTypeCfg &&cfg);

    void setSignatureCfg(SignatureCfg &&cfg);

    std::vector<Block> getBlocks() const;

    uint32_t getID() const;

    uint8_t getParameterID() const;

    const AuTypeCfg &getAlignmentInfo() const;

    uint32_t getReadCount() const;

    core::record::ClassType getClass() const { return au_type; }

   private:
    void preWrite(util::BitWriter &writer) const;

    uint32_t access_unit_ID;
    uint8_t num_blocks;
    uint8_t parameter_set_ID;
    core::record::ClassType au_type;
    uint32_t reads_count;
    boost::optional<MmCfg> mm_cfg;
    boost::optional<RefCfg> ref_cfg;
    boost::optional<AuTypeCfg> au_Type_U_Cfg;
    boost::optional<SignatureCfg> signature_config;

    std::vector<Block> blocks;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ACCESS_UNIT_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
