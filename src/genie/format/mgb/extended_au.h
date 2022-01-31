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
 * @brief
 */
class ExtendedAu {
 private:
    uint64_t extended_AU_start_position;  //!< @brief
    uint64_t extended_AU_end_position;    //!< @brief

    uint8_t posSize;  //!< @brief Internal

 public:
    bool operator==(const ExtendedAu& other) const {
        return extended_AU_start_position == other.extended_AU_start_position &&
               extended_AU_end_position == other.extended_AU_end_position && posSize == other.posSize;
    }

    /**
     * @brief
     * @param _extended_AU_start_position
     * @param _extended_AU_end_position
     * @param _posSize
     */
    ExtendedAu(uint64_t _extended_AU_start_position, uint64_t _extended_AU_end_position, uint8_t _posSize);

    /**
     * @brief
     * @param _posSize
     * @param reader
     */
    ExtendedAu(uint8_t _posSize, util::BitReader& reader);

    /**
     * @brief
     */
    virtual ~ExtendedAu() = default;

    /**
     * @brief
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
