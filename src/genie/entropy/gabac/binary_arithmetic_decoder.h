/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_BINARY_ARITHMETIC_DECODER_H_
#define SRC_GENIE_ENTROPY_GABAC_BINARY_ARITHMETIC_DECODER_H_

// -----------------------------------------------------------------------------

#include "genie/entropy/gabac/bit_input_stream.h"
#include "genie/entropy/gabac/context_model.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief
 */
class BinaryArithmeticDecoder {
public:
 /**
  * @brief
  * @param bit_input_stream
  */
 explicit BinaryArithmeticDecoder(const BitInputStream& bit_input_stream);

 /**
  * @brief
  * @param context_model
  * @return
  */
 unsigned int DecodeBin(ContextModel* context_model);

 /**
  * @brief
  * @param num_bins
  * @return
  */
 unsigned int DecodeBinsEp(unsigned int num_bins);

 /**
  * @brief
  */
 void DecodeBinTrm();

 /**
  * @brief
  */
 void Reset();

 /**
  * @brief
  */
 void Start();

 /**
  * @brief
  * @return
  */
 size_t Close();

private:
 BitInputStream bit_input_stream_;  //!< @brief
 int num_bits_needed_ = 0;          //!< @brief
 unsigned int range_ = 0;           //!< @brief
 unsigned int value_ = 0;           //!< @brief
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_BINARY_ARITHMETIC_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------