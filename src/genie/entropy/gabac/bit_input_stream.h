/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_BIT_INPUT_STREAM_H_
#define SRC_GENIE_ENTROPY_GABAC_BIT_INPUT_STREAM_H_

// -----------------------------------------------------------------------------

#include "genie/util/block_stepper.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief
 */
class BitInputStream {
 public:
  /**
   * @brief
   * @param bitstream
   */
  explicit BitInputStream(util::DataBlock* bitstream);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] unsigned int GetNumBitsUntilByteAligned() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] size_t GetNumBytesRead() const;

  /**
   * @brief
   * @return
   */
  unsigned char ReadByte();

  /**
   * @brief
   * @param num_bits
   * @return
   */
  unsigned int Read(unsigned int num_bits);

  /**
   * @brief
   * @param num_bytes
   */
  [[maybe_unused]] void SkipBytes(unsigned int num_bytes);

  /**
   * @brief
   */
  void Reset();

 private:
  util::DataBlock* bitstream_;
  util::BlockStepper reader_;
  unsigned char held_bits_;
  unsigned int num_held_bits_;
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_BIT_INPUT_STREAM_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
