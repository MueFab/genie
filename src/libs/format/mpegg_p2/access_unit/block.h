#ifndef GENIE_BLOCK_H
#define GENIE_BLOCK_H

// -----------------------------------------------------------------------------------------------------------------

#include <format/block-payload.h>
#include <cstdint>
#include <vector>
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {

/**
 * ISO 23092-2 Section 3.4.1.2 table 20 + 3.4.1.2.1 table 21
 */
class Block {
   private:
    /**
     * Incorporated (Simplification): ISO 23092-2 Section 3.4.1.2 table 20
     *
     * ------------------------------------------------------------------------------------------------------------ */
    uint8_t reserved;             //!< Line 2
    uint8_t descriptor_ID;        //!< Line 3
    uint8_t reserved_2;           //!< Line 4
    uint32_t block_payload_size;  //!< Line 5

    /**
     * Incorporated (Simplification): ISO 23092-2 Section 3.4.1.2.1 table 21
     *
     * ------------------------------------------------------------------------------------------------------------ */
    BlockPayloadSet::DescriptorPayload payload;

   public:
    Block();
    Block(uint8_t _descriptor_ID, BlockPayloadSet::DescriptorPayload _payload);
    Block(util::BitReader &reader) {
        reserved = reader.read(1);
        descriptor_ID = reader.read(7);
        reserved_2 = reader.read(3);
        block_payload_size = reader.read(29);

        /*   for(size_t i = 0; i < block_payload_size; ++i) {
               reader.read(8);
           } */

        payload = BlockPayloadSet::DescriptorPayload(coding::GenDesc(descriptor_ID), block_payload_size, reader);

        reader.flush();
    }
    virtual ~Block() = default;
    virtual void write(util::BitWriter &writer) const;

    BlockPayloadSet::DescriptorPayload &&movePayload() { return std::move(payload); }

    uint8_t getDescriptorID() const { return descriptor_ID; };
};
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_BLOCK_H
