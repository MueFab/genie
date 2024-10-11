/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/alignment_external/none.h"
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::alignment_external {

// ---------------------------------------------------------------------------------------------------------------------

None::None() : AlignmentExternal(Type::NONE) {}

// ---------------------------------------------------------------------------------------------------------------------

void None::write(util::BitWriter &writer) const { AlignmentExternal::write(writer); }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<AlignmentExternal> None::clone() const {
    auto ret = std::make_unique<None>();
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_external

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
