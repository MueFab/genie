/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "split-alignment-same-rec.h"
#include <backbone/mpegg_rec/split-alignment.h>
#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <util/make_unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace mpegg_rec {

// ---------------------------------------------------------------------------------------------------------------------

SplitAlignmentSameRec::SplitAlignmentSameRec()
    : SplitAlignment(SplitAlignment::SplitAlignmentType::SAME_REC), delta(0), alignment() {}

// ---------------------------------------------------------------------------------------------------------------------

SplitAlignmentSameRec::SplitAlignmentSameRec(int64_t _delta, Alignment _alignment)
    : SplitAlignment(SplitAlignment::SplitAlignmentType::SAME_REC), delta(_delta), alignment(std::move(_alignment)) {}

// ---------------------------------------------------------------------------------------------------------------------

SplitAlignmentSameRec::SplitAlignmentSameRec(uint8_t as_depth, util::BitReader &reader)
    : SplitAlignment(SplitAlignment::SplitAlignmentType::SAME_REC) {
    delta = reader.read(48);
    alignment = Alignment(as_depth, reader);
}

// ---------------------------------------------------------------------------------------------------------------------

void SplitAlignmentSameRec::write(util::BitWriter &writer) const {
    SplitAlignment::write(writer);
    writer.write(uint64_t(delta), 48);
    alignment.write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

const Alignment &SplitAlignmentSameRec::getAlignment() const { return alignment; }

// ---------------------------------------------------------------------------------------------------------------------

int64_t SplitAlignmentSameRec::getDelta() const { return delta; }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<SplitAlignment> SplitAlignmentSameRec::clone() const {
    auto ret = util::make_unique<SplitAlignmentSameRec>();
    ret->delta = this->delta;
    ret->alignment = this->alignment;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------