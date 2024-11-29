/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_COMPUTED_REF_EXTENDED_H_
#define SRC_GENIE_CORE_PARAMETER_COMPUTED_REF_EXTENDED_H_

// -----------------------------------------------------------------------------

#include <cstdint>

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter {

/**
 * @brief
 */
class ComputedRefExtended final {
  uint8_t cr_pad_size_;       //!< @brief
  uint32_t cr_buf_max_size_;  //!< @brief

 public:
  /**
   * @brief
   * @param ext
   * @return
   */
  bool operator==(const ComputedRefExtended& ext) const;

  /**
   * @brief
   * @param cr_pad_size
   * @param cr_buf_max_size
   */
  ComputedRefExtended(uint8_t cr_pad_size, uint32_t cr_buf_max_size);

  /**
   * @brief
   */
  ~ComputedRefExtended() = default;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint32_t GetBufMaxSize() const;

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_COMPUTED_REF_EXTENDED_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
