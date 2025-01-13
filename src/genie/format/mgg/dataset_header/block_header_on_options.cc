/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_header/block_header_on_options.h"

namespace genie::format::mgg::dataset_header {

// -----------------------------------------------------------------------------

bool BlockHeaderOnOptions::operator==(const BlockHeaderOnOptions& other) const {
  return mit_flag_ == other.mit_flag_ && cc_mode_flag_ == other.cc_mode_flag_;
}

// -----------------------------------------------------------------------------

BlockHeaderOnOptions::BlockHeaderOnOptions(const bool mit_flag,
                                           const bool cc_mode_flag)
    : mit_flag_(mit_flag), cc_mode_flag_(cc_mode_flag) {}

// -----------------------------------------------------------------------------

BlockHeaderOnOptions::BlockHeaderOnOptions(util::BitReader& reader) {
  mit_flag_ = reader.Read<bool>(1);
  cc_mode_flag_ = reader.Read<bool>(1);
}

// -----------------------------------------------------------------------------

void BlockHeaderOnOptions::Write(util::BitWriter& writer) const {
  writer.WriteBits(mit_flag_, 1);
  writer.WriteBits(cc_mode_flag_, 1);
}

// -----------------------------------------------------------------------------

bool BlockHeaderOnOptions::GetMitFlag() const { return mit_flag_; }

// -----------------------------------------------------------------------------

bool BlockHeaderOnOptions::GetCcFlag() const { return cc_mode_flag_; }

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
