/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/raw_reference_seq.h"

#include <string>
#include <utility>

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

// -----------------------------------------------------------------------------

uint16_t RawReferenceSequence::GetSeqId() const { return sequence_id_; }

// -----------------------------------------------------------------------------

uint64_t RawReferenceSequence::GetStart() const { return seq_start_; }

// -----------------------------------------------------------------------------

uint64_t RawReferenceSequence::GetEnd() const { return seq_end_; }

// -----------------------------------------------------------------------------

std::string& RawReferenceSequence::GetSequence() { return ref_sequence_; }

// -----------------------------------------------------------------------------

const std::string& RawReferenceSequence::GetSequence() const {
  return ref_sequence_;
}

// -----------------------------------------------------------------------------

RawReferenceSequence::RawReferenceSequence(util::BitReader& reader,
                                           const bool header_only) {
  sequence_id_ = reader.Read<uint16_t>();
  seq_start_ = reader.Read<uint64_t>(40);
  seq_end_ = reader.Read<uint64_t>(40);
  if (!header_only) {
    ref_sequence_.resize(seq_end_ - seq_start_ + 1);
    reader.ReadAlignedBytes(&ref_sequence_[0], seq_end_ - seq_start_ + 1);
  } else {
    UTILS_DIE_IF(!reader.IsByteAligned(), "Bitreader not aligned");
    reader.SkipAlignedBytes(seq_end_ - seq_start_ + 1);
  }
}

// -----------------------------------------------------------------------------

RawReferenceSequence::RawReferenceSequence(const uint16_t sequence_id,
                                           const uint64_t seq_start,
                                           std::string&& ref_sequence)
    : sequence_id_(sequence_id),
      seq_start_(seq_start),
      seq_end_(seq_start + ref_sequence.length()),
      ref_sequence_(std::move(ref_sequence)) {}

// -----------------------------------------------------------------------------

void RawReferenceSequence::Write(util::BitWriter& writer) const {
  writer.WriteBits(sequence_id_, 16);
  writer.WriteBits(seq_start_, 40);
  writer.WriteBits(seq_start_ + ref_sequence_.length() - 1, 40);
  writer.WriteAlignedBytes(ref_sequence_.data(), ref_sequence_.length());
}

// -----------------------------------------------------------------------------

bool RawReferenceSequence::IsIdUnique(const RawReferenceSequence& s) const {
  return sequence_id_ != s.sequence_id_;
}

// -----------------------------------------------------------------------------

uint64_t RawReferenceSequence::GetTotalSize() const {
  constexpr uint64_t header_size =
      (16 + 40 + 40) / 8;  // sequence_ID, seq_start, seq_end
  return ref_sequence_.length() + header_size;  // Including \0
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
