/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/bsc/subsequence.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::bsc {

// -----------------------------------------------------------------------------

Subsequence::Subsequence(const uint8_t output_symbol_size)
    : output_symbol_size_(output_symbol_size) {}

// -----------------------------------------------------------------------------

[[maybe_unused]] [[maybe_unused]] uint8_t Subsequence::GetOutputSymbolSize()
    const {
  return output_symbol_size_;
}

// -----------------------------------------------------------------------------

void Subsequence::Write(util::BitWriter& writer) const {
  writer.WriteBits(output_symbol_size_, 6);
}

// -----------------------------------------------------------------------------

bool Subsequence::operator==(const Subsequence& rhs) const {
  return output_symbol_size_ == rhs.output_symbol_size_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
