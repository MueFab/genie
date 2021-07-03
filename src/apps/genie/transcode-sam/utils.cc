/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/transcode-sam/utils.h"
#include "genie/core/record/alignment_split/other-rec.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace transcode_sam {

// ---------------------------------------------------------------------------------------------------------------------

bool compare(const genie::core::record::Record& r1, const genie::core::record::Record& r2) {
    if (r1.getAlignments().empty()) {
        return false;
    }
    if (r2.getAlignments().empty()) {
        return true;
    }
    return r1.getAlignments().front().getPosition() < r2.getAlignments().front().getPosition();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace transcode_sam
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
