/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_BLOCK_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/format/mpegg_p1/block_header.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class Block {
 private:
    BlockHeader header;                    //!< @brief
    genie::util::DataBlock block_payload;  //!< @brief block_payload_size (implicite), block_payload[]

 public:
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
    genie::util::DataBlock&& movePayload();

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

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
