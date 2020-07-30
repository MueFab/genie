/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_PART1_BLOCK_HEADER_H
#define GENIE_PART1_BLOCK_HEADER_H

#include <vector>
#include <cstdint>

#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class BlockHeader{
   private:
    uint8_t descriptor_ID;
    uint32_t block_payload_size;

   public:
    /**
     *
     * @param _desc_ID
     * @param payload
     */
    BlockHeader(uint8_t _desc_ID, std::vector<uint8_t>& payload);

    /**
     *
     * @return
     */
    uint8_t getDescriptorID() const;

    /**
     *
     * @return
     */
    uint32_t getBlockPayloadSize() const;

    /**
     *
     * @return
     */
    uint64_t getLength() const;

    /**
     *
     * @param bit_writer
     */
    void writeToFile(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif //GENIE_PART1_BLOCK_HEADER_H
