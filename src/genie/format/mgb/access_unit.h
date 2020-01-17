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
 * ISO 23092-2 Section 3.4.1.1 table 19
 */
class AccessUnit : public core::parameter::DataUnit {
   public:
    explicit AccessUnit(const std::map<size_t, std::unique_ptr<core::parameter::ParameterSet>> &parameterSets,
                        util::BitReader &bitReader);

    void write(util::BitWriter &writer) const override;

    AccessUnit(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, core::record::ClassType _au_type,
               uint32_t _reads_count, DatasetType dataset_type, uint8_t posSize, uint8_t signatureSize,
               uint32_t multiple_signature_base);

    void addBlock(Block block);

    void setMmCfg(std::unique_ptr<MmCfg> cfg);

    void setRefCfg(std::unique_ptr<RefCfg> cfg);

    void setAuTypeCfg(std::unique_ptr<AuTypeCfg> cfg);

    void setSignatureCfg(std::unique_ptr<SignatureCfg> cfg);

    std::vector<Block> getBlocks() const;

    uint32_t getID() const;

    uint8_t getParameterID() const;

    const AuTypeCfg &getAlignmentInfo() const;

    uint32_t getReadCount() const;

   private:
    void preWrite(util::BitWriter &writer) const;

    /**
     * Incorporated (Simplification): ISO 23092-2 Section 3.1 table 3
     *
     * ------------------------------------------------------------------------------------------------------------ */
    uint8_t reserved : 3;  //!< Line 13
    // uint32_t data_unit_size : 29;  //!< Line 14

    /** ----------------------------------------------------------------------------------------------------------- */

    /**
     * ISO 23092-2 Section 3.4.1.1 table 19
     *
     * ------------------------------------------------------------------------------------------------------------- */

    uint32_t access_unit_ID;                         //!< Line 2
    uint8_t num_blocks;                              //!< Line 3
    uint8_t parameter_set_ID;                        //!< Line 4
    core::record::ClassType au_type;                 //!< : 4;  Line 5
    uint32_t reads_count;                            //!< Line 6
    std::unique_ptr<MmCfg> mm_cfg;                   //!< Lines 7 to 10
    std::unique_ptr<RefCfg> ref_cfg;                 //!< Lines 11 to 15
    std::unique_ptr<AuTypeCfg> au_Type_U_Cfg;        //!< Lines 16 to 25
    std::unique_ptr<SignatureCfg> signature_config;  //!< Lines 26 to 41

    /** Padding in write() //!< Line 42 + 43 */

    /** ----------------------------------------------------------------------------------------------------------- */

    /**
     * ISO 23092-2 Section 3.4.1 table 18
     *
     * ------------------------------------------------------------------------------------------------------------- */
    std::vector<Block> blocks;  //!< Lines 3 to 5
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ACCESS_UNIT_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------