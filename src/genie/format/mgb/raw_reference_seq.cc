/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "raw_reference_seq.h"
#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

RawReferenceSequence::RawReferenceSequence(uint16_t _sequence_ID, uint64_t _seq_start, std::string&& _ref_sequence)
    : sequence_ID(_sequence_ID), seq_start(_seq_start), ref_sequence(std::move(_ref_sequence)) {}

// ---------------------------------------------------------------------------------------------------------------------

void RawReferenceSequence::write(util::BitWriter &writer) const {
    writer.write(sequence_ID, 16);
    writer.write(seq_start, 40);
    writer.write(seq_start + ref_sequence.length(), 40);
    for (const auto &a : ref_sequence) {
        writer.write(a, 8);
    }
    writer.write('\0', 8);
}

// ---------------------------------------------------------------------------------------------------------------------

bool RawReferenceSequence::isIdUnique(const RawReferenceSequence &s) const { return sequence_ID != s.sequence_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t RawReferenceSequence::getTotalSize() const {
    uint64_t HEADER_SIZE = (16 + 40 + 40) / 8;          // sequence_ID, seq_start, seq_end
    return (ref_sequence.length() + 1) + HEADER_SIZE;  // Including \0
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------