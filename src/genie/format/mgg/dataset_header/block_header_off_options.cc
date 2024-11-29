/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_header/block_header_off_options.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

// -----------------------------------------------------------------------------
bool BlockHeaderOffOptions::operator==(
    const BlockHeaderOffOptions& other) const {
  return ordered_blocks_flag_ == other.ordered_blocks_flag_;
}

// -----------------------------------------------------------------------------
BlockHeaderOffOptions::BlockHeaderOffOptions(const bool ordered_blocks_flag)
    : ordered_blocks_flag_(ordered_blocks_flag) {}

// -----------------------------------------------------------------------------
BlockHeaderOffOptions::BlockHeaderOffOptions(util::BitReader& reader) {
  ordered_blocks_flag_ = reader.Read<bool>(1);
}

// -----------------------------------------------------------------------------
bool BlockHeaderOffOptions::GetOrderedBlocksFlag() const {
  return ordered_blocks_flag_;
}

// -----------------------------------------------------------------------------
void BlockHeaderOffOptions::Write(util::BitWriter& writer) const {
  writer.WriteBits(ordered_blocks_flag_, 1);
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
