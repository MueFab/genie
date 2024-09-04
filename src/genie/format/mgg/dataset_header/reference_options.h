/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_HEADER_REFERENCE_OPTIONS_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_HEADER_REFERENCE_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

/**
 * @brief
 */
class ReferenceOptions {
 private:
    uint8_t reference_ID;              //!< @brief
    std::vector<uint16_t> seq_ID;      //!< @brief
    std::vector<uint32_t> seq_blocks;  //!< @brief

 public:
    /**
     * @brief
     * @param _old
     * @param _new
     */
    void patchRefID(uint8_t _old, uint8_t _new);

    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const ReferenceOptions& other) const;

    /**
     * @brief
     * @param reader
     */
    explicit ReferenceOptions(genie::util::BitReader& reader);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief
     */
    ReferenceOptions();

    /**
     * @brief
     * @param _reference_ID
     * @param _seq_id
     * @param blocks
     */
    void addSeq(uint8_t _reference_ID, uint16_t _seq_id, uint32_t blocks);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<uint16_t>& getSeqIDs() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<uint32_t>& getSeqBlocks() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getReferenceID() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_HEADER_REFERENCE_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
