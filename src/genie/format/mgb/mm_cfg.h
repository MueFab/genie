/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_MM_CFG_H_
#define SRC_GENIE_FORMAT_MGB_MM_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgb {

/**
 * @brief ISO 23092-2 Section 3.4.1.1 table 19 lines 7 to 10
 */
class MmCfg {
 private:
    uint16_t mm_threshold;  //!< Line 8
    uint32_t mm_count;      //!< Line 9

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const MmCfg &other) const;

    /**
     * @brief
     * @param _mm_threshold
     * @param _mm_count
     */
    MmCfg(uint16_t _mm_threshold, uint32_t _mm_count);

    /**
     * @brief
     */
    MmCfg();

    /**
     * @brief
     * @param reader
     */
    explicit MmCfg(util::BitReader &reader);

    /**
     * @brief
     */
    virtual ~MmCfg() = default;

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter &writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgb

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_MM_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
