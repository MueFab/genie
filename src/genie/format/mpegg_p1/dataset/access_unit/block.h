/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_BLOCK_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/format/mpegg_p1/dataset/access_unit/block_header.h>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include "genie/format/mpegg_p1/dataset/class_description.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
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
     * @param reader
     * @param fhd
     */
    explicit Block(util::BitReader& reader);

    Block(genie::core::GenDesc _desc_id, genie::util::DataBlock payload)
        : header(false, _desc_id, 0, payload.getRawSize()), block_payload(std::move(payload)) {}

    /**
     * @brief
     * @return
     */
    uint32_t getPayloadSize() const;

    /**
     * @brief
     * @return
     */
    const genie::util::DataBlock& getPayload() const {
        return block_payload;
    }

    genie::util::DataBlock&& movePayload() {
        return std::move(block_payload);
    }

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

    genie::core::GenDesc getDescID() const {
        return header.getDescriptorID();
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
