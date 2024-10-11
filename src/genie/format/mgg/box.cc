/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/format/mgg/box.h"
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

void Box::print_offset(std::ostream& output, const uint8_t depth, const uint8_t max_depth,
                       const std::string& msg) const {
    if (depth > max_depth) {
        return;
    }
    for (int i = 0; i < depth; ++i) {
        output << "\t";
    }
    output << msg << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

void Box::print_debug(std::ostream& output, const uint8_t depth, const uint8_t max_depth) const {
    (void)output;
    (void)depth;
    (void)max_depth;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
