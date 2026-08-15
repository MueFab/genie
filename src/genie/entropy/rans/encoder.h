/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_RANS_ENCODER_H_
#define SRC_GENIE_ENTROPY_RANS_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <array>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <ostream>
#include <vector>
#include "genie/entropy/rans/rans64.h"
#include "genie/entropy/rans/commons.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::rans {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief RANS Encoder class responsible for encoding data using RANS.
 */
class RANSEncoder {
 public:
  RANSEncoder() = default;

  /**
    * @brief Encodes data from an input stream and writes the compressed data to an output stream.
    * @param input The input stream containing uncompressed data.
    * @param output The output stream to write compressed data to.
    */
  void encode(std::istream& input, std::ostream& output, bool is_table_stored, uint32_t num_interleavings);
 private:
  uint32_t PROB_BITS = 14;
  uint32_t PROB_SCALE = 1 << PROB_BITS;
};
}  // namespace genie::entropy::rans

#endif  // SRC_GENIE_ENTROPY_RANS_ENCODER_H_
