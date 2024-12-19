/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_MISMATCH_DECODER_H_
#define SRC_GENIE_CORE_MISMATCH_DECODER_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <memory>

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class MismatchDecoder {
 public:
  /**
   * @brief
   */
  virtual ~MismatchDecoder() = default;

  /**
   * @brief
   * @param ref
   * @return
   */
  virtual uint64_t DecodeMismatch(uint64_t ref) = 0;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] virtual bool DataLeft() const = 0;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] virtual std::unique_ptr<MismatchDecoder> Copy() const = 0;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_MISMATCH_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
