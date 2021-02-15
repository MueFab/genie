/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_EXTENDED_AU_H_
#define SRC_GENIE_FORMAT_MGB_EXTENDED_AU_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 * ISO 23092-2 Section 3.4.1.1 table 19 lines 21 to 24
 */
class ExtendedAu {
 private:
    uint64_t extended_AU_start_position;  //!< Line 22
    uint64_t extended_AU_end_position;    //!< Line 23

    uint8_t posSize;  //!< Internal

 public:
    /**
     *
     * @param _extended_AU_start_position
     * @param _extended_AU_end_position
     * @param _posSize
     */
    ExtendedAu(uint64_t _extended_AU_start_position, uint64_t _extended_AU_end_position, uint8_t _posSize);

    /**
     *
     * @param _posSize
     * @param reader
     */
    ExtendedAu(uint8_t _posSize, util::BitReader& reader);

    /**
     *
     */
    virtual ~ExtendedAu() = default;

    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_EXTENDED_AU_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
