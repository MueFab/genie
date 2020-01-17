/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_REF_CFG_H
#define GENIE_REF_CFG_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <cstdint>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 * ISO 23092-2 Section 3.4.1.1 table 19 lines 11 to 15
 */
class RefCfg {
   private:
    uint16_t ref_sequence_ID;     //!< Line 12
    uint64_t ref_start_position;  //!< Line 13
    uint64_t ref_end_position;    //!< Line 14

    uint8_t posSize;  //!< internal
   public:
    RefCfg(uint16_t _ref_sequence_ID, uint64_t _ref_start_position, uint64_t _ref_end_position, uint8_t _posSize);
    explicit RefCfg(uint8_t _posSize);
    RefCfg(uint8_t _posSize, util::BitReader &reader);
    virtual ~RefCfg() = default;

    virtual void write(util::BitWriter &writer);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_REF_CFG_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------