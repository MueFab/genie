/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "split-alignment-other-rec.h"
#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <util/make_unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace mpegg_rec {

// ---------------------------------------------------------------------------------------------------------------------

SplitAlignmentOtherRec::SplitAlignmentOtherRec(uint64_t _split_pos, uint16_t _split_seq_ID)
    : SplitAlignment(SplitAlignment::SplitAlignmentType::OTHER_REC),
      split_pos(_split_pos),
      split_seq_ID(_split_seq_ID) {}

// ---------------------------------------------------------------------------------------------------------------------

SplitAlignmentOtherRec::SplitAlignmentOtherRec(util::BitReader &reader)
    : SplitAlignment(SplitAlignment::SplitAlignmentType::OTHER_REC),
      split_pos(reader.read(40)),
      split_seq_ID(reader.read<uint16_t>()) {}

// ---------------------------------------------------------------------------------------------------------------------

SplitAlignmentOtherRec::SplitAlignmentOtherRec()
    : SplitAlignment(SplitAlignment::SplitAlignmentType::OTHER_REC), split_pos(0), split_seq_ID(0) {}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t SplitAlignmentOtherRec::getNextPos() const { return split_pos; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t SplitAlignmentOtherRec::getNextSeq() const { return split_seq_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void SplitAlignmentOtherRec::write(util::BitWriter &writer) const {
    writer.write(split_pos, 40);
    writer.write(split_seq_ID, 16);
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<SplitAlignment> SplitAlignmentOtherRec::clone() const {
    auto ret = util::make_unique<SplitAlignmentOtherRec>();
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------