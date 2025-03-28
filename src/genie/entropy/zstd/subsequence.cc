/**
 * Copyright 2018-2024 The Genie Authors.
 * @file subsequence.cc
 * @brief Implementation of Zstd-based subsequence handling for Genie.
 *
 * Provides the `Subsequence` class for managing output symbol size,
 * serialization, and equality comparison.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/zstd/subsequence.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::zstd {

// -----------------------------------------------------------------------------

Subsequence::Subsequence(const uint8_t output_symbol_size)
    : output_symbol_size_(output_symbol_size) {}

// -----------------------------------------------------------------------------

uint8_t Subsequence::GetOutputSymbolSize() const { return output_symbol_size_; }

// -----------------------------------------------------------------------------

void Subsequence::write(util::BitWriter& writer) const {
  writer.WriteBits(output_symbol_size_, 6);
}

// -----------------------------------------------------------------------------

bool Subsequence::operator==(const Subsequence& rhs) const {
  return output_symbol_size_ == rhs.output_symbol_size_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::zstd

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
