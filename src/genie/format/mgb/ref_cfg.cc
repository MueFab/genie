/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/ref_cfg.h"

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

// -----------------------------------------------------------------------------
bool RefCfg::operator==(const RefCfg& other) const {
  return ref_sequence_id_ == other.ref_sequence_id_ &&
         ref_start_position_ == other.ref_start_position_ &&
         ref_end_position_ == other.ref_end_position_ &&
         pos_size_ == other.pos_size_;
}

// -----------------------------------------------------------------------------
uint16_t RefCfg::GetSeqId() const { return ref_sequence_id_; }

// -----------------------------------------------------------------------------
uint64_t RefCfg::GetStart() const { return ref_start_position_; }

// -----------------------------------------------------------------------------
uint64_t RefCfg::GetEnd() const { return ref_end_position_; }

// -----------------------------------------------------------------------------
void RefCfg::Write(util::BitWriter& writer) const {
  writer.WriteBits(ref_sequence_id_, 16);
  writer.WriteBits(ref_start_position_, pos_size_);
  writer.WriteBits(ref_end_position_, pos_size_);
}

// -----------------------------------------------------------------------------
RefCfg::RefCfg(const uint16_t ref_sequence_id,
               const uint64_t ref_start_position,
               const uint64_t ref_end_position, const uint8_t pos_size)
    : ref_sequence_id_(ref_sequence_id),
      ref_start_position_(ref_start_position),
      ref_end_position_(ref_end_position),
      pos_size_(pos_size) {}

// -----------------------------------------------------------------------------
RefCfg::RefCfg(const uint8_t pos_size) : RefCfg(0, 0, 0, pos_size) {}

// -----------------------------------------------------------------------------
RefCfg::RefCfg(const uint8_t pos_size, util::BitReader& reader)
    : pos_size_(pos_size) {
  ref_sequence_id_ = reader.Read<uint16_t>();
  ref_start_position_ = reader.Read<uint64_t>(pos_size_);
  ref_end_position_ = reader.Read<uint64_t>(pos_size_);
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
