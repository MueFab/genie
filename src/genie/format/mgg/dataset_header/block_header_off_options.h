/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_HEADER_BLOCK_HEADER_OFF_OPTIONS_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_HEADER_BLOCK_HEADER_OFF_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

/**
 * @brief
 */
struct BlockHeaderOffOptions {
 private:
    bool ordered_blocks_flag;  //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const BlockHeaderOffOptions& other) const;

    /**
     * @brief
     * @param _ordered_blocks_flag
     */
    explicit BlockHeaderOffOptions(bool _ordered_blocks_flag);

    /**
     * @brief
     * @param reader
     */
    explicit BlockHeaderOffOptions(util::BitReader& reader);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool getOrderedBlocksFlag() const;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_HEADER_BLOCK_HEADER_OFF_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
