/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/alignment_external/none.h"
#include <memory>
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace alignment_external {

// ---------------------------------------------------------------------------------------------------------------------

None::None() : AlignmentExternal(AlignmentExternal::Type::NONE) {}

// ---------------------------------------------------------------------------------------------------------------------

void None::write(util::BitWriter &writer) const { AlignmentExternal::write(writer); }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<AlignmentExternal> None::clone() const {
    auto ret = util::make_unique<None>();
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace alignment_external
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
