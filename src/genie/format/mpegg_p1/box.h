/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_BOX_H
#define GENIE_BOX_H

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class Box {
 protected:
    virtual void print_offset(std::ostream& output, uint8_t depth, uint8_t max_depth, const std::string& msg) const {
        if (depth > max_depth) {
            return;
        }
        for(int i = 0; i < depth; ++i) {
            output << "\t";
        }
        output << msg << std::endl;
    }
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
     */
    virtual void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const {
        (void) output;
        (void) depth;
        (void) max_depth;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_BOX_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------