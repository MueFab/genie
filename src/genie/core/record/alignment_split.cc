/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "alignment_split.h"
#include "genie/core/record/alignment_split/other-rec.h"
#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/core/record/alignment_split/unpaired.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

AlignmentSplit::AlignmentSplit(Type _split_alignment) : split_alignment(_split_alignment) {}

// ---------------------------------------------------------------------------------------------------------------------

void AlignmentSplit::write(util::BitWriter &writer) const { writer.WriteBypassBE(split_alignment); }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<AlignmentSplit> AlignmentSplit::factory(uint8_t as_depth, util::BitReader &reader) {
    Type type = reader.ReadAlignedInt<Type>();
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
