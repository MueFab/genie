/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/alignment_split/unpaired.h"
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::alignment_split {

// ---------------------------------------------------------------------------------------------------------------------

Unpaired::Unpaired() : AlignmentSplit(AlignmentSplit::Type::UNPAIRED) {}

// ---------------------------------------------------------------------------------------------------------------------

void Unpaired::write(util::BitWriter &writer) const { AlignmentSplit::write(writer); }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<AlignmentSplit> Unpaired::clone() const {
    auto ret = std::make_unique<Unpaired>();
    ret->split_alignment = this->split_alignment;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_split

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
