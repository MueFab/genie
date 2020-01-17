/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MM_CFG_H
#define GENIE_MM_CFG_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <cstdint>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 * ISO 23092-2 Section 3.4.1.1 table 19 lines 7 to 10
 */
class MmCfg {
   private:
    uint16_t mm_threshold;  //!< Line 8
    uint32_t mm_count;      //!< Line 9

   public:
    MmCfg(uint16_t _mm_threshold, uint32_t _mm_count);
    MmCfg();
    explicit MmCfg(util::BitReader &reader);
    virtual ~MmCfg() = default;

    virtual void write(util::BitWriter &writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MM_CFG_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------