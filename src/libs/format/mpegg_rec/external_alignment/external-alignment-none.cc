/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "external-alignment-none.h"
#include <util/make_unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_rec {

// ---------------------------------------------------------------------------------------------------------------------

ExternalAlignmentNone::ExternalAlignmentNone() : ExternalAlignment(ExternalAlignment::MoreAlignmentInfoType::NONE) {}

// ---------------------------------------------------------------------------------------------------------------------

void ExternalAlignmentNone::write(util::BitWriter &writer) const { ExternalAlignment::write(writer); }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<ExternalAlignment> ExternalAlignmentNone::clone() const {
    auto ret = util::make_unique<ExternalAlignmentNone>();
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------