/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/alignment_external/other-rec.h"
#include <memory>
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::alignment_external {

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec(uint64_t _next_pos, uint16_t _next_seq_ID)
    : AlignmentExternal(Type::OTHER_REC), next_pos(_next_pos), next_seq_ID(_next_seq_ID) {}

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec(util::BitReader &reader)
    : AlignmentExternal(Type::OTHER_REC),
      next_pos(reader.readAlignedInt<uint64_t, 5>()),
      next_seq_ID(reader.readAlignedInt<uint16_t>()) {}

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec() : AlignmentExternal(Type::OTHER_REC), next_pos(0), next_seq_ID(0) {}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t OtherRec::getNextPos() const { return next_pos; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t OtherRec::getNextSeq() const { return next_seq_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void OtherRec::write(util::BitWriter &writer) const {
    AlignmentExternal::write(writer);
    writer.writeAlignedInt<uint64_t, 5>(next_pos);
    writer.writeAlignedInt(next_seq_ID);
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<AlignmentExternal> OtherRec::clone() const {
    auto ret = std::make_unique<OtherRec>();
    ret->next_pos = this->next_pos;
    ret->next_seq_ID = this->next_seq_ID;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_external

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
