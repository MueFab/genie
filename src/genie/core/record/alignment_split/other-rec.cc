/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "other-rec.h"
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>

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
      split_pos(reader.read(40)),
      split_seq_ID(reader.read<uint16_t>()) {}

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec() : AlignmentSplit(AlignmentSplit::Type::OTHER_REC), split_pos(0), split_seq_ID(0) {}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t OtherRec::getNextPos() const { return split_pos; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t OtherRec::getNextSeq() const { return split_seq_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void OtherRec::write(util::BitWriter &writer) const {
    writer.write(split_pos, 40);
    writer.write(split_seq_ID, 16);
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<AlignmentSplit> OtherRec::clone() const {
    auto ret = util::make_unique<OtherRec>();
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace alignment_split
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------