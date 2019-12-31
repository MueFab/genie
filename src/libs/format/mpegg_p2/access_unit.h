#ifndef GENIE_ACCESS_UNIT_H
#define GENIE_ACCESS_UNIT_H

// -----------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <vector>
#include "util/make_unique.h"

#include <gabac/gabac.h>
#include <format/mpegg_rec/mpegg-record.h>

#include "access_unit/au_type_cfg.h"
#include "access_unit/block.h"
#include "access_unit/mm_cfg.h"
#include "access_unit/ref_cfg.h"
#include "access_unit/signature_cfg.h"
#include "data_unit.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    namespace mpegg_p2 {
/**
 * ISO 23092-2 Section 3.4.1.1 table 19
 */
        class AccessUnit : public DataUnit {
        public:
            explicit AccessUnit(util::BitReader *bitReader);

            void write(util::BitWriter *writer) const override;

            AccessUnit(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, mpegg_rec::ClassType _au_type,
                       uint32_t _reads_count,
                       DatasetType dataset_type, uint8_t posSize, uint8_t signatureSize,
                       uint32_t multiple_signature_base);

            void addBlock(Block block);

            void setMmCfg(std::unique_ptr<MmCfg> cfg);

            void setRefCfg(std::unique_ptr<RefCfg> cfg);

            void setAuTypeCfg(std::unique_ptr<AuTypeCfg> cfg);

            void setSignatureCfg(std::unique_ptr<SignatureCfg> cfg);

        private:
            void preWrite(util::BitWriter *writer) const;

            /**
             * Incorporated (Simplification): ISO 23092-2 Section 3.1 table 3
             *
             * ------------------------------------------------------------------------------------------------------------ */
            uint8_t reserved : 3;          //!< Line 13
        //    uint32_t data_unit_size : 29;  //!< Line 14

            /** ----------------------------------------------------------------------------------------------------------- */

            /**
             * ISO 23092-2 Section 3.4.1.1 table 19
             *
             * ------------------------------------------------------------------------------------------------------------- */

            uint32_t access_unit_ID : 32;                    //!< Line 2
            uint8_t num_blocks : 8;                          //!< Line 3
            uint8_t parameter_set_ID : 8;                    //!< Line 4
            mpegg_rec::ClassType au_type;                                  //!< : 4;  Line 5
            uint32_t reads_count : 32;                       //!< Line 6
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
    }
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ACCESS_UNIT_H
