/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ENTROPY_RANS_ENCODER_H
#define GENIE_ENTROPY_RANS_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include "rans64.h"
#include "commons.h"

#include <boost/mpl/size.hpp>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <ostream>
#include <vector>
#include <istream>
#include <ostream>
#include <array>
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
} // namespace genie::entropy::rans

#endif // GENIE_ENTROPY_RANS_ENCODER_H
