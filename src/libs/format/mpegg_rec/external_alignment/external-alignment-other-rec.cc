/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "external-alignment-other-rec.h"
#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <util/make_unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_rec {

// ---------------------------------------------------------------------------------------------------------------------

ExternalAlignmentOtherRec::ExternalAlignmentOtherRec(uint64_t _next_pos, uint16_t _next_seq_ID)
    : ExternalAlignment(ExternalAlignment::MoreAlignmentInfoType::OTHER_REC),
      next_pos(_next_pos),
      next_seq_ID(_next_seq_ID) {}

// ---------------------------------------------------------------------------------------------------------------------

ExternalAlignmentOtherRec::ExternalAlignmentOtherRec(util::BitReader &reader)
    : ExternalAlignment(ExternalAlignment::MoreAlignmentInfoType::OTHER_REC),
      next_pos(reader.read(40)),
      next_seq_ID(reader.read<uint16_t>()) {}

// ---------------------------------------------------------------------------------------------------------------------

ExternalAlignmentOtherRec::ExternalAlignmentOtherRec()
    : ExternalAlignment(ExternalAlignment::MoreAlignmentInfoType::OTHER_REC), next_pos(0), next_seq_ID(0) {}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ExternalAlignmentOtherRec::getNextPos() const { return next_pos; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t ExternalAlignmentOtherRec::getNextSeq() const { return next_seq_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void ExternalAlignmentOtherRec::write(util::BitWriter &writer) const {
    ExternalAlignment::write(writer);
    writer.write(next_pos, 40);
    writer.write(next_seq_ID, 16);
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<ExternalAlignment> ExternalAlignmentOtherRec::clone() const {
    auto ret = util::make_unique<ExternalAlignmentOtherRec>();
    ret->next_pos = this->next_pos;
    ret->next_seq_ID = this->next_seq_ID;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------