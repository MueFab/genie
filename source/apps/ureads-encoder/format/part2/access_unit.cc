#include "access_unit.h"
#include "ureads-encoder/exceptions.h"

#include <sstream>
#include <gabac/data-block.h>

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    AccessUnit::AccessUnit(
            uint32_t _access_unit_ID,
            uint8_t _parameter_set_ID,
            AuType _au_type,
            uint32_t _reads_count,
            DatasetType dataset_type
    ) : DataUnit(DataUnitType::ACCESS_UNIT),
        reserved(0),
        data_unit_size(0),
        access_unit_ID(_access_unit_ID),
        num_blocks(0),
        parameter_set_ID(_parameter_set_ID),
        au_type(_au_type),
        reads_count(_reads_count),
        mm_cfg(nullptr),
        ref_cfg(nullptr),
        au_Type_U_Cfg(nullptr),
        signature_config(nullptr),
        blocks(0),
        internalBitCounter(0) {
        if (au_type == AuType::N_TYPE_AU || au_type == AuType::M_TYPE_AU) {
           // mm_cfg =make_unique<MmCfg>(); // TODO: Fill for types N and M
            GENIE_THROW_RUNTIME_EXCEPTION("Types N and M not supported");
        }
        if (dataset_type == DatasetType::REFERENCE) {
         //   ref_cfg = make_unique<RefCfg>(); // TODO: Fill for dataset_type 2
            GENIE_THROW_RUNTIME_EXCEPTION("Dataset type reference not supported");
        }
        if (au_type != AuType::U_TYPE_AU) {
          //  au_Type_U_Cfg = make_unique<AuTypeCfg>(); // TODO: Fill for ALIGNED data
            GENIE_THROW_RUNTIME_EXCEPTION("AU type != U not supported");
        } else {
/*        if (multiple_signature_base != 0) { // TODO: Check
            GENIE_THROW_RUNTIME_EXCEPTION("multiple_signature_base not supported");
        }*/
        }

        std::stringstream s;
        BitWriter bw(&s);
        write(&bw);

        internalBitCounter = bw.getBitsWritten();
        data_unit_size = internalBitCounter / 8;
        if (internalBitCounter % 8) {
            data_unit_size += 1;
        }
    }

    // -----------------------------------------------------------------------------------------------------------------

    void AccessUnit::write(BitWriter *writer) {
        DataUnit::write(writer);
        writer->write(reserved, 3);
        writer->write(data_unit_size, 29);
        writer->write(access_unit_ID, 32);
        writer->write(num_blocks, 8);
        writer->write(parameter_set_ID, 8);
        writer->write(uint8_t(au_type), 4);
        writer->write(reads_count, 32);
        if(mm_cfg){
            mm_cfg->write(writer);
        }
        if(ref_cfg) {
            ref_cfg->write(writer);
        }
        if(au_Type_U_Cfg) {
            au_Type_U_Cfg->write(writer);
        }
        if(signature_config) {
            signature_config->write(writer);
        }
        writer->flush(); // Zero bytes
        for (auto &i : blocks) {
            i->write(writer);
        }
    }

    // -----------------------------------------------------------------------------------------------------------------

    void AccessUnit::addBlock(std::unique_ptr<Block> block) {
        data_unit_size += block->getTotalSize();
        ++num_blocks;
        blocks.push_back(std::move(block));
    }
}