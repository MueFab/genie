/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_OFFSET_H_
#define SRC_GENIE_FORMAT_MGG_OFFSET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

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
    [[nodiscard]] const std::string& getSubkey() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint64_t getOffset() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_OFFSET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
