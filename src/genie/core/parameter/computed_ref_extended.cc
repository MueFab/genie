/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/parameter/computed_ref_extended.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter {

// -----------------------------------------------------------------------------

bool ComputedRefExtended::operator==(const ComputedRefExtended& ext) const {
  return cr_pad_size_ == ext.cr_pad_size_ &&
         cr_buf_max_size_ == ext.cr_buf_max_size_;
}

// -----------------------------------------------------------------------------

ComputedRefExtended::ComputedRefExtended(const uint8_t cr_pad_size,
                                         const uint32_t cr_buf_max_size)
    : cr_pad_size_(cr_pad_size), cr_buf_max_size_(cr_buf_max_size) {}

// -----------------------------------------------------------------------------

uint32_t ComputedRefExtended::GetBufMaxSize() const { return cr_buf_max_size_; }

// -----------------------------------------------------------------------------

void ComputedRefExtended::Write(util::BitWriter& writer) const {
  writer.WriteBits(cr_pad_size_, 8);
  writer.WriteBits(cr_buf_max_size_, 24);
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
