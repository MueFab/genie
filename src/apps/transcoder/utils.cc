/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/transcoder/utils.h"
#include <genie/core/record/alignment_split/other-rec.h>

namespace genie {
namespace transcoder {

uint64_t getMinPos(const genie::core::record::Record& r) {
    auto alignments = r.getAlignments();

    if (alignments.empty()) {
        return 0;
    }

    uint64_t first_pos = alignments.front().getPosition();
    if (r.isRead1First()) {
        return first_pos;
    } else {
        auto& split = r.getAlignments().front().getAlignmentSplits().front();
        //        UTILS_DIE_IF(split->getType() != genie::core::record::AlignmentSplit::Type::SAME_REC,
        //                     "Only same rec split alignments supported");
        //        return first_pos + dynamic_cast<const
        //        genie::core::record::alignment_split::SameRec&>(*split).getDelta();

        switch (split->getType()) {
            case genie::core::record::AlignmentSplit::Type::SAME_REC: {
                auto delta = dynamic_cast<const genie::core::record::alignment_split::SameRec&>(*split).getDelta();
                return delta > 0 ? first_pos : first_pos + delta;
            }

            case genie::core::record::AlignmentSplit::Type::OTHER_REC: {
                auto second_pos =
                    dynamic_cast<const genie::core::record::alignment_split::OtherRec&>(*split).getNextPos();
                return first_pos < second_pos ? first_pos : second_pos;
            }

            default:
                UTILS_DIE("Unknown type found while computing minimum position");
        }
    }
}

bool compare(const genie::core::record::Record& r1, const genie::core::record::Record& r2) {
    if (r1.getAlignments().empty()) {
        return false;
    }
    if (r2.getAlignments().empty()) {
        return true;
    }
    return getMinPos(r1) < getMinPos(r2);
}

}  // namespace transcoder
}  // namespace genie
