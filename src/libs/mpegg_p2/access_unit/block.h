#ifndef GENIE_BLOCK_H
#define GENIE_BLOCK_H

// -----------------------------------------------------------------------------------------------------------------

#include <backbone/block-payload.h>
#include <cstdint>
#include <vector>
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace genie {
namespace mpegg_p2 {

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
    genie::BlockPayloadSet::DescriptorPayload payload;

   public:
    Block();
    Block(uint8_t _descriptor_ID, genie::BlockPayloadSet::DescriptorPayload _payload);
    explicit Block(util::BitReader &reader);
    virtual ~Block() = default;
    virtual void write(util::BitWriter &writer) const;

    genie::BlockPayloadSet::DescriptorPayload &&movePayload();

    uint8_t getDescriptorID() const;
};
}  // namespace mpegg_p2
}  // namespace genie
// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_BLOCK_H
