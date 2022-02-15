/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/alignment_split/other-rec.h"
#include <memory>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace alignment_split {

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec() : AlignmentSplit(AlignmentSplit::Type::OTHER_REC), split_pos(0), split_seq_ID(0),
                       reverse_comp(0), ecigar_string(){}
// ---------------------------------------------------------------------------------------------------------------------

// TODO: initialization of ecigar_string and its default value
OtherRec::OtherRec(uint64_t _split_pos, uint16_t _split_seq_ID, uint8_t _reverse_comp, std::string &&_ecigar_string)
    : AlignmentSplit(AlignmentSplit::Type::OTHER_REC), split_pos(_split_pos), split_seq_ID(_split_seq_ID),
      reverse_comp(_reverse_comp), ecigar_string(std::move(_ecigar_string)){}

// ---------------------------------------------------------------------------------------------------------------------

OtherRec::OtherRec(bool extended_alignment_info, util::BitReader &reader)
    : AlignmentSplit(AlignmentSplit::Type::OTHER_REC),
      split_pos(reader.readBypassBE<uint64_t, 5>()),
      split_seq_ID(reader.readBypassBE<uint16_t>()),
      reverse_comp(0),
      ecigar_string(){

    if (extended_alignment_info){
        reverse_comp = reader.readBypassBE<uint8_t>();

        ecigar_string.resize(reader.readBypassBE<uint32_t, 3>());
        reader.readBypass(&ecigar_string[0], ecigar_string.size());
    }
}


// ---------------------------------------------------------------------------------------------------------------------

uint64_t OtherRec::getNextPos() const { return split_pos; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t OtherRec::getNextSeq() const { return split_seq_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Alignment::getRComp() const { return this->reverse_comp; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string &Alignment::getECigar() const { return ecigar_string; }

// ---------------------------------------------------------------------------------------------------------------------

void OtherRec::write(util::BitWriter &writer) const {
    AlignmentSplit::write(writer);
    writer.writeBypassBE<uint64_t, 5>(split_pos);
    writer.writeBypassBE(split_seq_ID);
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
