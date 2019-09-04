#ifndef GENIE_ACCESS_UNIT_H
#define GENIE_ACCESS_UNIT_H

// -----------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <vector>

#include <gabac/gabac.h>

#include "data_unit.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    struct MmCfg {
        uint16_t mm_threshold : 16;
        uint32_t mm_count : 32;

        virtual void write(BitWriter &writer);
    };

    // -----------------------------------------------------------------------------------------------------------------

    struct RefCfg {
        uint16_t ref_sequence_ID : 16;
        uint64_t ref_start_position; // TODO: Size
        uint64_t ref_end_position; // TODO: Size

        virtual void write(BitWriter &writer);
    };

    // -----------------------------------------------------------------------------------------------------------------

    struct ExtendedAu {
        uint64_t extended_AU_start_position; // TODO: Size
        uint64_t extended_AU_end_position; // TODO: Size

        virtual void write(BitWriter &writer);
    };

    // -----------------------------------------------------------------------------------------------------------------

    struct AuTypeCfg {
        uint16_t sequence_ID : 16;
        uint64_t AU_start_position; // TODO: Size;
        uint64_t AU_end_position; // TODO: Size;
        std::vector<ExtendedAu> extended_AU;

        virtual void write(BitWriter &writer);
    };

    // -----------------------------------------------------------------------------------------------------------------

    class Block {
    private:
        // Header
        uint8_t reserved : 1;
        uint8_t descriptor_ID : 7;
        uint8_t reserved_2 : 3;
        uint32_t block_payload_size : 29;

        std::vector<uint8_t> payload; // TODO: adjust gabac stream size

    public:
        virtual void write(BitWriter &writer);

        uint32_t getTotalSize() {
            return block_payload_size + 5;
        }

        Block(uint8_t _descriptor_ID, std::vector<uint8_t> *_payload);

        Block();
    };

    // -----------------------------------------------------------------------------------------------------------------

    class AccessUnit : public DataUnit {
    public:
        void write(BitWriter *writer) override;

        AccessUnit(uint32_t _access_unit_ID,
                   uint8_t _parameter_set_ID,
                   AuType _au_type,
                   uint32_t _reads_count,
                   DatasetType dataset_type);

        void addBlock(std::unique_ptr<Block> block);

    private:
        // data unit
        uint8_t reserved : 3;
        uint32_t data_unit_size : 29;

        // Header
        uint32_t access_unit_ID : 32;
        uint8_t num_blocks : 8;
        uint8_t parameter_set_ID : 8;
        AuType au_type; // : 4
        uint32_t reads_count : 32;
        std::vector<MmCfg> mm_cfg;
        std::vector<RefCfg> ref_cfg;
        std::vector<AuTypeCfg> au_Type_U_Cfg;
        std::vector<uint16_t> num_signatures; //  : 16
        std::vector<uint64_t> u_cluster_signatures; // TODO: size

        // --- padding ---

        // Blocks
        std::vector<std::unique_ptr<Block>> blocks;

        uint64_t internalBitCounter;
    };

    // -----------------------------------------------------------------------------------------------------------------

    AccessUnit createQuickAccessUnit(uint32_t access_unit_id,
                                     uint8_t parameter_set_id,
                                     uint32_t reads_count,
                                     std::vector<std::vector<gabac::DataBlock>> *data
    );
}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_ACCESS_UNIT_H
