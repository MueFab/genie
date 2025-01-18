/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief This file contains the definition of the Subsequence class used for
 * BSC entropy coding.
 * @details The Subsequence class represents a configuration for handling
 * specific subsequence elements in a block-based entropy encoder.
 * @copyright This file is part of Genie
 * See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_BSC_SUBSEQUENCE_H_
#define SRC_GENIE_ENTROPY_BSC_SUBSEQUENCE_H_

// -----------------------------------------------------------------------------

#include <cstdint>

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::bsc {

/**
 * @brief Class representing a subsequence configuration for BSC entropy coding.
 */
class Subsequence {
 public:
  /**
   * @brief Construct a new Subsequence object with a specified output symbol
   * Size.
   * @param output_symbol_size Size of the output symbols in bits.
   */
  explicit Subsequence(uint8_t output_symbol_size);

  /**
   * @brief Get the output symbol Size for this subsequence.
   * @return Output symbol Size in bits.
   */
  [[maybe_unused]] [[nodiscard]] uint8_t GetOutputSymbolSize() const;

  /**
   * @brief Write the subsequence configuration to a BitWriter.
   * @param writer BitWriter object to write the configuration to.
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief Compare this subsequence configuration to another for equality.
   * @param rhs Another Subsequence object to compare to.
   * @return True if both configurations are equal.
   */
  bool operator==(const Subsequence& rhs) const;

 private:
  uint8_t output_symbol_size_;  //!< @brief Output symbol Size in bits.
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_BSC_SUBSEQUENCE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
