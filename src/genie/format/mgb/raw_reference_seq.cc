/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/raw_reference_seq.h"
#include <string>
#include <utility>
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgb {

// ---------------------------------------------------------------------------------------------------------------------

uint16_t RawReferenceSequence::getSeqID() const { return sequence_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t RawReferenceSequence::getStart() const { return seq_start; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t RawReferenceSequence::getEnd() const { return seq_end; }

// ---------------------------------------------------------------------------------------------------------------------

std::string& RawReferenceSequence::getSequence() { return ref_sequence; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& RawReferenceSequence::getSequence() const { return ref_sequence; }

// ---------------------------------------------------------------------------------------------------------------------

RawReferenceSequence::RawReferenceSequence(util::BitReader& reader, bool headerOnly) {
    sequence_ID = reader.read<uint16_t>();
    seq_start = reader.read<uint64_t>(40);
    seq_end = reader.read<uint64_t>(40);
    if (!headerOnly) {
        ref_sequence.resize(seq_end - seq_start + 1);
        reader.readAlignedBytes(&ref_sequence[0], seq_end - seq_start + 1);
    } else {
        UTILS_DIE_IF(!reader.isByteAligned(), "Bitreader not aligned");
        reader.skipAlignedBytes(seq_end - seq_start + 1);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

RawReferenceSequence::RawReferenceSequence(uint16_t _sequence_ID, uint64_t _seq_start, std::string&& _ref_sequence)
    : sequence_ID(_sequence_ID),
      seq_start(_seq_start),
      seq_end(_seq_start + _ref_sequence.length()),
      ref_sequence(std::move(_ref_sequence)) {}

// ---------------------------------------------------------------------------------------------------------------------

void RawReferenceSequence::write(util::BitWriter& writer) const {
    writer.writeBits(sequence_ID, 16);
    writer.writeBits(seq_start, 40);
    writer.writeBits(seq_start + ref_sequence.length() - 1, 40);
    writer.writeAlignedBytes(ref_sequence.data(), ref_sequence.length());
}

// ---------------------------------------------------------------------------------------------------------------------

bool RawReferenceSequence::isIdUnique(const RawReferenceSequence& s) const { return sequence_ID != s.sequence_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t RawReferenceSequence::getTotalSize() const {
    uint64_t HEADER_SIZE = (16 + 40 + 40) / 8;     // sequence_ID, seq_start, seq_end
    return (ref_sequence.length()) + HEADER_SIZE;  // Including \0
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgb

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
