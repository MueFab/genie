/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/alignment_split/other-rec.h"
#include <memory>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"


// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::alignment_split {

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec(uint64_t _split_pos, uint16_t _split_seq_ID)
    : AlignmentSplit(AlignmentSplit::Type::OTHER_REC), split_pos(_split_pos), split_seq_ID(_split_seq_ID) {}

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec(util::BitReader &reader)
    : AlignmentSplit(AlignmentSplit::Type::OTHER_REC),
      split_pos(reader.readBypassBE<uint64_t, 5>()),
      split_seq_ID(reader.readBypassBE<uint16_t>()) {}

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec() : AlignmentSplit(AlignmentSplit::Type::OTHER_REC), split_pos(0), split_seq_ID(0) {}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t OtherRec::getNextPos() const { return split_pos; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t OtherRec::getNextSeq() const { return split_seq_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void OtherRec::write(util::BitWriter &writer) const {
    AlignmentSplit::write(writer);
    writer.writeBypassBE<uint64_t, 5>(split_pos);
    writer.writeBypassBE(split_seq_ID);
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<AlignmentSplit> OtherRec::clone() const {
    auto ret = std::make_unique<OtherRec>();
    ret->split_pos = this->split_pos;
    ret->split_seq_ID = this->split_seq_ID;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_split

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
