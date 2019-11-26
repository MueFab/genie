#ifndef GENIE_BLOCK_H
#define GENIE_BLOCK_H

// -----------------------------------------------------------------------------------------------------------------

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
    uint8_t reserved : 1;              //!< Line 2
    uint8_t descriptor_ID : 7;         //!< Line 3
    uint8_t reserved_2 : 3;            //!< Line 4
    uint32_t block_payload_size : 29;  //!< Line 5

    /**
     * Incorporated (Simplification): ISO 23092-2 Section 3.4.1.2.1 table 21
     *
     * ------------------------------------------------------------------------------------------------------------ */
    std::vector<uint8_t> payload;  // TODO: adjust gabac stream size

   public:
    Block();
    Block(uint8_t _descriptor_ID, std::vector<uint8_t> *_payload);
    virtual ~Block() = default;
    virtual void write(util::BitWriter *writer);
    uint32_t getTotalSize();
};
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_BLOCK_H
