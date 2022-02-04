/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_BLOCK_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/payload.h"
#include "genie/format/mpegg_p1/block_header.h"
#include "genie/format/mpegg_p1/box.h"
#include "genie/format/mgb/block.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class Block : public Box {
 private:
    BlockHeader header;  //!< @brief
    core::Payload payload;

 public:
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override {
        print_offset(output, depth, max_depth, "* Block");
        print_offset(output, depth + 1, max_depth,
                     "Block descriptor ID: " + genie::core::getDescriptor(header.getDescriptorID()).name);
        print_offset(output, depth + 1, max_depth, "Block payload size: " + std::to_string(header.getPayloadSize()));
    }

    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const Block& other) const;

    /**
     * @brief
     * @param reader
     */
    explicit Block(util::BitReader& reader);

    /**
     * @brief
     * @param _desc_id
     * @param payload
     */
    Block(genie::core::GenDesc _desc_id, genie::util::DataBlock payload);

    explicit Block(format::mgb::Block b)
        : header(false, core::GenDesc(b.getDescriptorID()), 0, b.getPayloadUnparsed().getPayloadSize()),
          payload(std::move(b.getPayloadUnparsed())) {}

    /**
     * @brief
     * @return
     */
    uint32_t getPayloadSize() const;

    /**
     * @brief
     * @return
     */
    const genie::util::DataBlock& getPayload() const;

    /**
     * @brief
     * @return
     */
    core::Payload movePayload() { return std::move(boost::get<core::Payload>(payload)); }

    format::mgb::Block decapsulate() { return {header.getDescriptorID(), movePayload()}; }

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const override;

    /**
     * @brief
     * @return
     */
    genie::core::GenDesc getDescID() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
