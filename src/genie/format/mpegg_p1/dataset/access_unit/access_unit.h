/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef GENIE_PART1_ACCESS_UNIT_H
#define GENIE_PART1_ACCESS_UNIT_H

#include <cstdint>
#include <map>
#include <memory>
#include <vector>

#include <genie/format/mgb/access_unit.h>
#include <genie/core/parameter/data_unit.h>

#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/core/record/class-type.h"
#include <genie/format/mgb/mm_cfg.h>
#include <genie/format/mgb/au_type_cfg.h>
#include <genie/format/mgb/ref_cfg.h>
#include <genie/format/mgb/signature_cfg.h>

#include <genie/format/mpegg_p1/dataset/access_unit/block.h>

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 *
 */
class AUInformation {
   private:
    std::vector<uint8_t> AU_information_value;  //!<

   public:
    /**
     *
     */
    AUInformation();

    uint64_t getLength() const;

    void write(genie::util::BitWriter& bit_writer) const;
};

/**
*
*/
class AUProtection {
   private:
    std::vector<uint8_t> AU_protection_value;  //!<

   public:
    /**
     *
     */
    AUProtection();

    uint64_t getLength() const;

    void write(genie::util::BitWriter& bit_writer) const;

};

class AccessUnit: public core::parameter::DataUnit{
   private:

    /* ----- internal ----- */
    /// From Dataset/BlockHeaderFlags/block_header_flag
    bool block_header_flag;
    // already inside ref_cfg
    //uint8_t posSize;
    // already inside signature_config
    //uint8_t U_signature_size;

    /* ----- access_unit_header ----- */
    uint32_t access_unit_ID;                         //!<
    uint8_t num_blocks;                              //!<
    uint8_t parameter_set_ID;                        //!<
    core::record::ClassType au_type;                 //!<
    uint32_t reads_count;                            //!<

    std::unique_ptr<mgb::MmCfg> mm_cfg;
    std::unique_ptr<mgb::RefCfg> ref_cfg;
    std::unique_ptr<mgb::AuTypeCfg> au_Type_U_Cfg;        //!<
    std::unique_ptr<mgb::SignatureCfg> signature_config;  //!<

    /* ----- access_unit ----- */

    std::unique_ptr<AUInformation> AU_information;
    std::unique_ptr<AUProtection> AU_protection;

    std::list<Block> blocks;

   public:
    AccessUnit(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, core::record::ClassType _au_type,
               uint32_t _reads_count, DatasetType dataset_type, uint8_t posSize, uint8_t signatureSize,
               uint32_t multiple_signature_base);

    explicit AccessUnit(format::mgb::AccessUnit&& au);

    uint64_t getHeaderLength() const;

    void writeHeader(util::BitWriter& writer, bool write_dummy_length = false) const;

    uint64_t getLength() const;

    void write(genie::util::BitWriter& writer) const override;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif //GENIE_PART1_ACCESS_UNIT_H
