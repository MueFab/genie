/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "other-rec.h"
#include <memory>
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace alignment_split {

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec(uint64_t _split_pos, uint16_t _split_seq_ID)
    : AlignmentSplit(AlignmentSplit::Type::OTHER_REC), split_pos(_split_pos), split_seq_ID(_split_seq_ID) {}

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec(util::BitReader &reader)
    : AlignmentSplit(AlignmentSplit::Type::OTHER_REC),
      split_pos(reader.ReadAlignedInt<uint64_t, 5>()),
      split_seq_ID(reader.ReadAlignedInt<uint16_t>()) {}

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec() : AlignmentSplit(AlignmentSplit::Type::OTHER_REC), split_pos(0), split_seq_ID(0) {}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t OtherRec::getNextPos() const { return split_pos; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t OtherRec::getNextSeq() const { return split_seq_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void OtherRec::write(util::BitWriter &writer) const {
    AlignmentSplit::write(writer);
    writer.WriteBypassBE<uint64_t, 5>(split_pos);
    writer.WriteBypassBE(split_seq_ID);
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<AlignmentSplit> OtherRec::clone() const {
    auto ret = util::make_unique<OtherRec>();
    ret->split_pos = this->split_pos;
    ret->split_seq_ID = this->split_seq_ID;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace alignment_split
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
