/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/mm_cfg.h"

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

// -----------------------------------------------------------------------------
bool MmCfg::operator==(const MmCfg& other) const {
  return mm_threshold_ == other.mm_threshold_ && mm_count_ == other.mm_count_;
}

// -----------------------------------------------------------------------------
void MmCfg::Write(util::BitWriter& writer) const {
  writer.WriteBits(mm_threshold_, 16);
  writer.WriteBits(mm_count_, 32);
}

// -----------------------------------------------------------------------------
MmCfg::MmCfg(const uint16_t mm_threshold, const uint32_t mm_count)
    : mm_threshold_(mm_threshold), mm_count_(mm_count) {}

// -----------------------------------------------------------------------------
MmCfg::MmCfg() : mm_threshold_(0), mm_count_(0) {}

// -----------------------------------------------------------------------------
MmCfg::MmCfg(util::BitReader& reader) {
  mm_threshold_ = reader.Read<uint16_t>();
  mm_count_ = reader.Read<uint32_t>();
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
