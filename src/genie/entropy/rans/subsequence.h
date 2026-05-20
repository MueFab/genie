/**
* Copyright 2018-2024 The Genie Authors.
 * @file subsequence.h
 * @brief Header for rANS-based subsequence handling for Genie.
 *
 * Defines the `Subsequence` class for managing output symbol size,
 * serialization, and equality comparison.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */
#ifndef GENIE_ENTROPY_RANS_SUBSEQUENCE_H_
#define GENIE_ENTROPY_RANS_SUBSEQUENCE_H_

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------
namespace genie::entropy::rans {
// -----------------------------------------------------------------------------

/**
 * @brief Configuration for rANS entropy coder for one subsequence
 */
class Subsequence {
private:
  uint8_t output_symbol_size_;  //!< Number of bits per output symbol

public:
  /**
   * @brief Constructor
   * @param output_symbol_size Desired number of bits per output symbol
   */
  explicit Subsequence(uint8_t output_symbol_size);

  /**
   * @brief Get desired number of bits per output symbol
   * @return Number of bits per output symbol
   */
  uint8_t GetOutputSymbolSize() const;

  /**
   * @brief Write subsequence configuration to bit stream
   * @param writer Bit stream
   */
  void write(util::BitWriter& writer) const;

  /**
   * @brief Compare two subsequence configurations
   * @param rhs Other configuration
   * @return True if same
   */
  bool operator==(const Subsequence& rhs) const;
};

// -----------------------------------------------------------------------------
}  // namespace genie::entropy::rans
// -----------------------------------------------------------------------------

#endif  // GENIE_ENTROPY_RANS_SUBSEQUENCE_H_