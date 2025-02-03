/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_HEADER_BLOCK_HEADER_ON_OPTIONS_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_HEADER_BLOCK_HEADER_ON_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace dataset_header {

/**
 * @brief
 */
class BlockHeaderOnOptions {
 private:
    bool mit_flag;      //!< @brief
    bool cc_mode_flag;  //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const BlockHeaderOnOptions& other) const;

    /**
     * @brief
     * @param _mit_flag
     * @param _cc_mode_flag
     */
    BlockHeaderOnOptions(bool _mit_flag, bool _cc_mode_flag);

    /**
     * @brief
     * @param reader
     */
    explicit BlockHeaderOnOptions(genie::util::BitReader& reader);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    bool getMITFlag() const;

    /**
     * @brief
     * @return
     */
    bool getCCFlag() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace dataset_header
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_HEADER_BLOCK_HEADER_ON_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
