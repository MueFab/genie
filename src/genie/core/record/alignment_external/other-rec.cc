/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/alignment_external/other-rec.h"
#include <memory>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"


// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::alignment_external {

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec(uint64_t _next_pos, uint16_t _next_seq_ID)
    : AlignmentExternal(AlignmentExternal::Type::OTHER_REC), next_pos(_next_pos), next_seq_ID(_next_seq_ID) {}

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec(util::BitReader &reader)
    : AlignmentExternal(AlignmentExternal::Type::OTHER_REC),
      next_pos(reader.readBypassBE<uint64_t, 5>()),
      next_seq_ID(reader.readBypassBE<uint16_t>()) {}

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec() : AlignmentExternal(AlignmentExternal::Type::OTHER_REC), next_pos(0), next_seq_ID(0) {}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t OtherRec::getNextPos() const { return next_pos; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t OtherRec::getNextSeq() const { return next_seq_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void OtherRec::write(util::BitWriter &writer) const {
    AlignmentExternal::write(writer);
    writer.writeBypassBE<uint64_t, 5>(next_pos);
    writer.writeBypassBE(next_seq_ID);
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
