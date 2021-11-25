/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/alignment_split/same-rec.h"
#include <utility>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace alignment_split {

// ---------------------------------------------------------------------------------------------------------------------

SameRec::SameRec() : AlignmentSplit(AlignmentSplit::Type::SAME_REC), delta(0), alignment() {}

// ---------------------------------------------------------------------------------------------------------------------

SameRec::SameRec(int64_t _delta, Alignment _alignment)
    : AlignmentSplit(AlignmentSplit::Type::SAME_REC), delta(_delta), alignment(std::move(_alignment)) {}

// ---------------------------------------------------------------------------------------------------------------------

SameRec::SameRec(uint8_t as_depth, util::BitReader &reader)
    : AlignmentSplit(AlignmentSplit::Type::SAME_REC),
      delta(reader.readBypassBE<int64_t, 6>()),
      alignment(as_depth, reader) {}

// ---------------------------------------------------------------------------------------------------------------------

void SameRec::write(util::BitWriter &writer) const {
    AlignmentSplit::write(writer);
    writer.writeBypassBE<int64_t, 6>(delta);
    alignment.write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

const Alignment &SameRec::getAlignment() const { return alignment; }

// ---------------------------------------------------------------------------------------------------------------------

int64_t SameRec::getDelta() const { return delta; }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<AlignmentSplit> SameRec::clone() const {
    auto ret = util::make_unique<SameRec>();
    ret->delta = this->delta;
    ret->alignment = this->alignment;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace alignment_split
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
