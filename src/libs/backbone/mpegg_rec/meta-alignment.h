/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_META_ALIGNMENT_H
#define GENIE_META_ALIGNMENT_H

// ---------------------------------------------------------------------------------------------------------------------

#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <cstdint>
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace mpegg_rec {

/**
 *
 */
class MetaAlignment {
    uint16_t seq_ID;   //!<
    uint8_t as_depth;  //!<

   public:
    /**
     *
     */
    MetaAlignment();

    /**
     *
     * @param _seq_ID
     * @param _as_depth
     */
    MetaAlignment(uint16_t _seq_ID, uint8_t _as_depth);

    /**
     *
     * @param reader
     */
    explicit MetaAlignment(util::BitReader &reader);

    /**
     *
     * @param write
     */
    virtual void write(util::BitWriter &write) const;

    /**
     *
     * @return
     */
    uint16_t getSeqID() const;

    /**
     *
     * @return
     */
    uint8_t getAsDepth() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_META_ALIGNMENT_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------