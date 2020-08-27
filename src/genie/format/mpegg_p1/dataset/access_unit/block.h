/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef GENIE_PART1_BLOCK_H
#define GENIE_PART1_BLOCK_H

#include <vector>
#include <genie/format/mgb/block.h>
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class Block: mgb::Block{
   public:
    /**
     *
     * @param _desc_ID
     * @param payload
     */
    explicit Block(uint8_t _desc_ID, core::AccessUnit::Descriptor && payload);

    /**
     *
     * @return
     */
    const std::vector<uint8_t>& getPayload() const;

    /**
     *
     * @return
     */
    uint8_t getID() const;

    /**
     *
     * @return
     */
    uint32_t getPayloadSize() const;

    /**
     *
     * @return
     */
    uint64_t getLength() const;

    /**
     *
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif //GENIE_PART1_BLOCK_H
