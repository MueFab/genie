/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_BOX_H_
#define SRC_GENIE_FORMAT_MGG_BOX_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief
 */
class Box {
 protected:
    /**
     * @brief
     * @param output
     * @param depth
     * @param max_depth
     * @param msg
     */
    virtual void print_offset(std::ostream& output, uint8_t depth, uint8_t max_depth, const std::string& msg) const;

 public:
    /**
     * @brief
     */
    virtual ~Box() = default;

    /**
     * @brief
     * @param bitWriter
     */
    virtual void write(genie::util::BitWriter& bitWriter) const = 0;

    /**
     * @brief
     * @param output
     * @param depth
     * @param max_depth
     */
    virtual void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_BOX_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
