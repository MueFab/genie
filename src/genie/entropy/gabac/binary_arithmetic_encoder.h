/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_BINARY_ARITHMETIC_ENCODER_H_
#define SRC_GENIE_ENTROPY_GABAC_BINARY_ARITHMETIC_ENCODER_H_

// -----------------------------------------------------------------------------

#include "genie/entropy/gabac/context_model.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief
 */
class BinaryArithmeticEncoder {
 public:
  /**
   * @brief
   * @param bit_output_stream
   */
  explicit BinaryArithmeticEncoder(const util::BitWriter& bit_output_stream);

  /**
   * @brief
   */
  ~BinaryArithmeticEncoder();

  /**
   * @brief
   * @param bin
   * @param context_model
   */
  void EncodeBin(unsigned int bin, ContextModel* context_model);

  /**
   * @brief
   * @param bin
   */
  void EncodeBinEp(unsigned int bin);

  /**
   * @brief
   * @param bins
   * @param num_bins
   */
  void EncodeBinsEp(unsigned int bins, unsigned int num_bins);

  /**
   * @brief
   * @param bin
   */
  void EncodeBinTrm(unsigned int bin);

  /**
   * @brief
   */
  void Start();

  /**
   * @brief
   */
  void Flush();

 private:
  /**
   * @brief
   */
  void Finish();

  /**
   * @brief
   */
  void WriteOut();

  util::BitWriter bit_output_stream_;  //!< @brief

  unsigned char buffered_byte_;  //!< @brief

  unsigned int low_;  //!< @brief

  unsigned int num_bits_left_;  //!< @brief

  int num_buffered_bytes_;  //!< @brief

  unsigned int range_;  //!< @brief
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_BINARY_ARITHMETIC_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
