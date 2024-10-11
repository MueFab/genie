/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/alignment-split.h"
#include <memory>
#include "genie/core/record/alignment_split/other-rec.h"
#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/core/record/alignment_split/unpaired.h"
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

// ---------------------------------------------------------------------------------------------------------------------

AlignmentSplit::AlignmentSplit(const Type _split_alignment) : split_alignment(_split_alignment) {}

// ---------------------------------------------------------------------------------------------------------------------

void AlignmentSplit::write(util::BitWriter &writer) const { writer.writeAlignedInt(split_alignment); }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<AlignmentSplit> AlignmentSplit::factory(uint8_t as_depth, util::BitReader &reader) {
    const Type type = reader.readAlignedInt<Type>();
    switch (type) {
        case Type::SAME_REC:
            return std::make_unique<alignment_split::SameRec>(as_depth, reader);
        case Type::OTHER_REC:
            return std::make_unique<alignment_split::OtherRec>(reader);
        case Type::UNPAIRED:
            return std::make_unique<alignment_split::Unpaired>();
        default:
            UTILS_DIE("Invalid SplitAlignmentType");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

AlignmentSplit::Type AlignmentSplit::getType() const { return split_alignment; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
