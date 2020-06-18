/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ALIGNMENT_SHARED_DATA_H
#define GENIE_ALIGNMENT_SHARED_DATA_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <cstdint>
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

/**
 *
 */
class AlignmentSharedData {
    uint16_t seq_ID;   //!<
    uint8_t as_depth;  //!<

   public:
    /**
     *
     */
    AlignmentSharedData();

    /**
     *
     * @param _seq_ID
     * @param _as_depth
     */
    AlignmentSharedData(uint16_t _seq_ID, uint8_t _as_depth);

    /**
     *
     * @param reader
     */
    explicit AlignmentSharedData(util::BitReader &reader);

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

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ALIGNMENT_SHARED_DATA_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------