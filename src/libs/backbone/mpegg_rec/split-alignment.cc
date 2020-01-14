/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "split-alignment.h"
#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <util/exceptions.h>
#include <util/make_unique.h>
#include "split_alignment/split-alignment-other-rec.h"
#include "split_alignment/split-alignment-same-rec.h"
#include "split_alignment/split-alignment-unpaired.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace mpegg_rec {

// ---------------------------------------------------------------------------------------------------------------------

SplitAlignment::SplitAlignment(SplitAlignmentType _split_alignment) : split_alignment(_split_alignment) {}

// ---------------------------------------------------------------------------------------------------------------------

void SplitAlignment::write(util::BitWriter &writer) const { writer.write(uint8_t(split_alignment), 8); }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<SplitAlignment> SplitAlignment::factory(uint8_t as_depth, util::BitReader &reader) {
    auto type = SplitAlignmentType(reader.read(8));
    switch (type) {
        case SplitAlignmentType::SAME_REC:
            return util::make_unique<SplitAlignmentSameRec>(as_depth, reader);
        case SplitAlignmentType::OTHER_REC:
            return util::make_unique<SplitAlignmentOtherRec>(reader);
        case SplitAlignmentType::UNPAIRED:
            return util::make_unique<SplitAlignmentUnpaired>();
        default:
            UTILS_DIE("Invalid SplitAlignmentType");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

SplitAlignment::SplitAlignmentType SplitAlignment::getType() const { return split_alignment; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------