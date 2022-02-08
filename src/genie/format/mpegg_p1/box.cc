/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/format/mpegg_p1/box.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

void Box::print_offset(std::ostream& output, uint8_t depth, uint8_t max_depth, const std::string& msg) const {
    if (depth > max_depth) {
        return;
    }
    for (int i = 0; i < depth; ++i) {
        output << "\t";
    }
    output << msg << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

void Box::print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const {
    (void)output;
    (void)depth;
    (void)max_depth;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
