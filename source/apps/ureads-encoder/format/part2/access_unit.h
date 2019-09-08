#ifndef GENIE_ACCESS_UNIT_H
#define GENIE_ACCESS_UNIT_H

// -----------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <vector>
#include "ureads-encoder/format/part2/make_unique.h"

#include <gabac/gabac.h>

#include "ureads-encoder/format/part2/data_unit.h"
#include "ureads-encoder/format/part2/access_unit/block.h"
#include "ureads-encoder/format/part2/access_unit/mm_cfg.h"
#include "ureads-encoder/format/part2/access_unit/ref_cfg.h"
#include "ureads-encoder/format/part2/access_unit/au_type_cfg.h"
#include "ureads-encoder/format/part2/access_unit/signature_cfg.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    /**
    * ISO 23092-2 Section 3.4.1.1 table 19
    */
    class AccessUnit : public DataUnit {
    public:
        void write(BitWriter *writer) const override;

        AccessUnit(uint32_t _access_unit_ID,
                   uint8_t _parameter_set_ID,
                   AuType _au_type,
                   uint32_t _reads_count,
                   DatasetType dataset_type);

        void addBlock(std::unique_ptr<Block> block);

    private:

        void preWrite(BitWriter *writer) const;

        /**
         * Incorporated (Simplification): ISO 23092-2 Section 3.1 table 3
         *
         * ------------------------------------------------------------------------------------------------------------ */
        uint8_t reserved : 3; //!< Line 13
        uint32_t data_unit_size : 29; //!< Line 14

        /** ----------------------------------------------------------------------------------------------------------- */

        /**
        * ISO 23092-2 Section 3.4.1.1 table 19
        *
        * ------------------------------------------------------------------------------------------------------------- */

        uint32_t access_unit_ID : 32; //!< Line 2
        uint8_t num_blocks : 8; //!< Line 3
        uint8_t parameter_set_ID : 8; //!< Line 4
        AuType au_type; //!< : 4;  Line 5
        uint32_t reads_count : 32; //!< Line 6
        std::unique_ptr<MmCfg> mm_cfg; //!< Lines 7 to 10
        std::unique_ptr<RefCfg> ref_cfg; //!< Lines 11 to 15
        std::unique_ptr<AuTypeCfg> au_Type_U_Cfg; //!< Lines 16 to 25
        std::unique_ptr<SignatureCfg> signature_config; //!< Lines 26 to 41

        /** Padding in write() //!< Line 42 + 43 */

        /** ----------------------------------------------------------------------------------------------------------- */

        /**
        * ISO 23092-2 Section 3.4.1 table 18
        *
        * ------------------------------------------------------------------------------------------------------------- */
        std::vector<std::unique_ptr<Block>> blocks; //!< Lines 3 to 5
    };
}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_ACCESS_UNIT_H
