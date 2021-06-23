/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_BLOCK_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <list>
#include <vector>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class Block {
 private:
    uint8_t descriptor_ID;  //!< @brief

    std::list<uint8_t> block_payload;  //!< @brief block_payload_size (implicite), block_payload[]

 public:
    /**
     * @brief
     * @param reader
     */
    explicit Block(util::BitReader& reader);

    /**
     * @brief
     * @param _desc_ID
     * @param _block_payload
     */
    explicit Block(uint8_t _desc_ID, std::list<uint8_t>& _block_payload);

    /**
     * @brief
     * @return
     */
    uint8_t getDescID() const;

    /**
     * @brief
     * @return
     */
    uint32_t getPayloadSize() const;

    /**
     * @brief
     * @return
     */
    const std::list<uint8_t>& getPayload() const;

    /**
     * @brief
     * @return
     */
    uint64_t getHeaderLength() const;

    /**
     * @brief
     * @param writer
     */
    void writeHeader(util::BitWriter& writer) const;

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
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
