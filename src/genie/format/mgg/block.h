/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_BLOCK_H_
#define SRC_GENIE_FORMAT_MGG_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/payload.h"
#include "genie/format/mgb/block.h"
#include "genie/format/mgg/block_header.h"
#include "genie/format/mgg/box.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

/**
 * @brief
 */
class Block : public Box {
 private:
    BlockHeader header;     //!< @brief
    core::Payload payload;  //!< @brief

 public:
    /**
     * @brief
     * @param output
     * @param depth
     * @param max_depth
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;

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

    /**
     * @brief
     * @param b
     */
    explicit Block(format::mgb::Block b);

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
    core::Payload movePayload();

    /**
     * @brief
     * @return
     */
    format::mgb::Block decapsulate();

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

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
