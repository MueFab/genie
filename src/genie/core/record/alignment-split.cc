/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "alignment-split.h"
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>
#include <genie/util/runtime-exception.h>
#include "alignment_split/other-rec.h"
#include "alignment_split/same-rec.h"
#include "alignment_split/unpaired.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

AlignmentSplit::AlignmentSplit(Type _split_alignment) : split_alignment(_split_alignment) {}

// ---------------------------------------------------------------------------------------------------------------------

void AlignmentSplit::write(util::BitWriter &writer) const { writer.write(uint8_t(split_alignment), 8); }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<AlignmentSplit> AlignmentSplit::factory(uint8_t as_depth, util::BitReader &reader) {
    auto type = reader.read<Type>();
    switch (type) {
        case Type::SAME_REC:
            return util::make_unique<alignment_split::SameRec>(as_depth, reader);
        case Type::OTHER_REC:
            return util::make_unique<alignment_split::OtherRec>(reader);
        case Type::UNPAIRED:
            return util::make_unique<alignment_split::Unpaired>();
        default:
            UTILS_DIE("Invalid SplitAlignmentType");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

AlignmentSplit::Type AlignmentSplit::getType() const { return split_alignment; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------