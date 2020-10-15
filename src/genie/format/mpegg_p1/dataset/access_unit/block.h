/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_PART1_BLOCK_H
#define GENIE_PART1_BLOCK_H


#include <list>
#include <vector>

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/exception.h>

namespace genie {
namespace format {
namespace mpegg_p1 {

class Block{
   private:
    uint8_t descriptor_ID;

    // block_payload_size (implicite), block_payload[]
    std::list<uint8_t> block_payload;

   public:

    Block(util::BitReader &reader);

    /**
     *
     * @param _desc_ID
     * @param payload
     */
    explicit Block(uint8_t _desc_ID, std::list<uint8_t>& _block_payload);

    /**
     *
     * @return
     */
    uint8_t getDescID() const;

    /**
     *
     * @return
     */
    uint32_t getPayloadSize() const;

    /**
     *
     * @return
     */
    const std::list<uint8_t>& getPayload() const;

    /**
     *
     * @return
     */
    uint64_t getHeaderLength() const;

    /**
     *
     * @param writer
     */
    void writeHeader(util::BitWriter& writer) const;

    /**
     *
     * @return
     */
    uint64_t getLength() const;

    /**
     *
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif //GENIE_PART1_BLOCK_H
