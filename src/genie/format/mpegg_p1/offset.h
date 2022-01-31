/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_OFFSET_H
#define GENIE_OFFSET_H

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class Offset {
 private:
    std::string subkey;  //!< @brief
    uint64_t offset;     //!< @brief

 public:
    /**
     * @brief
     * @param bitReader
     */
    explicit Offset(util::BitReader& bitReader);

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    const std::string& getSubkey() const;

    /**
     * @brief
     * @return
     */
    uint64_t getOffset() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_OFFSET_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
