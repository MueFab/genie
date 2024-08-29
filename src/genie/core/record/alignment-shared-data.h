/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_SHARED_DATA_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_SHARED_DATA_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

/**
 * @brief
 */
class AlignmentSharedData {
    uint16_t seq_ID;   //!< @brief
    uint8_t as_depth;  //!< @brief

 public:
    /**
     * @brief
     */
    AlignmentSharedData();

    /**
     * @brief
     * @param _seq_ID
     * @param _as_depth
     */
    AlignmentSharedData(uint16_t _seq_ID, uint8_t _as_depth);

    /**
     * @brief
     * @param reader
     */
    explicit AlignmentSharedData(util::BitReader &reader);

    /**
     * @brief
     * @param write
     */
    virtual void write(util::BitWriter &write) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint16_t getSeqID() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getAsDepth() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_SHARED_DATA_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
