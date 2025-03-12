/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_header/reference_options.h"

#include <limits>
#include <vector>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

// -----------------------------------------------------------------------------

bool ReferenceOptions::operator==(const ReferenceOptions& other) const {
  return reference_id_ == other.reference_id_ && seq_id_ == other.seq_id_ &&
         seq_blocks_ == other.seq_blocks_;
}

// -----------------------------------------------------------------------------

ReferenceOptions::ReferenceOptions(util::BitReader& reader) {
  const auto seq_count = reader.Read<uint16_t>();
  if (!seq_count) {
    reference_id_ = std::numeric_limits<uint8_t>::max();
    return;
  }
  reference_id_ = reader.Read<uint8_t>();
  for (uint16_t i = 0; i < seq_count; ++i) {
    seq_id_.emplace_back(reader.Read<uint16_t>());
  }
  for (uint16_t i = 0; i < seq_count; ++i) {
    seq_blocks_.emplace_back(reader.Read<uint32_t>());
  }
}

// -----------------------------------------------------------------------------

void ReferenceOptions::Write(util::BitWriter& writer) const {
  writer.WriteBits(seq_id_.size(), 16);
  if (seq_id_.empty()) {
    return;
  }
  writer.WriteBits(reference_id_, 8);

  for (auto& i : seq_id_) {
    writer.WriteBits(i, 16);
  }

  for (auto& b : seq_blocks_) {
    writer.WriteBits(b, 32);
  }
}

// -----------------------------------------------------------------------------

ReferenceOptions::ReferenceOptions()
    : reference_id_(std::numeric_limits<uint8_t>::max()) {}

// -----------------------------------------------------------------------------

void ReferenceOptions::AddSeq(const uint8_t reference_id, const uint16_t seq_id,
                              const uint32_t blocks) {
  UTILS_DIE_IF(reference_id != reference_id_ && !seq_id_.empty(),
               "Mismatching ref id");
  reference_id_ = reference_id;
  seq_id_.push_back(seq_id);
  seq_blocks_.push_back(blocks);
}

// -----------------------------------------------------------------------------

const std::vector<uint16_t>& ReferenceOptions::GetSeqIDs() const {
  return seq_id_;
}

// -----------------------------------------------------------------------------

const std::vector<uint32_t>& ReferenceOptions::GetSeqBlocks() const {
  return seq_blocks_;
}

// -----------------------------------------------------------------------------

uint8_t ReferenceOptions::GetReferenceId() const { return reference_id_; }

// -----------------------------------------------------------------------------

void ReferenceOptions::PatchRefId(const uint8_t old_id, const uint8_t new_id) {
  if (reference_id_ == old_id || reference_id_ == 255) {
    reference_id_ = new_id;
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
