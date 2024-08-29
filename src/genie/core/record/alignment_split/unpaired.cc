/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/alignment_split/unpaired.h"
#include <memory>
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::alignment_split {

// ---------------------------------------------------------------------------------------------------------------------

Unpaired::Unpaired() : AlignmentSplit(AlignmentSplit::Type::UNPAIRED) {}

// ---------------------------------------------------------------------------------------------------------------------

void Unpaired::write(util::BitWriter &writer) const { AlignmentSplit::write(writer); }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<AlignmentSplit> Unpaired::clone() const {
    auto ret = util::make_unique<Unpaired>();
    ret->split_alignment = this->split_alignment;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_split

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
