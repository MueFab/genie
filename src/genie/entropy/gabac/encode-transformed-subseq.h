/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_ENCODE_TRANSFORMED_SUBSEQ_H_
#define SRC_GENIE_ENTROPY_GABAC_ENCODE_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------

#define NOMINMAX
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>
#include "genie/entropy/paramcabac/transformed-subseq.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

class DataBlock;
}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief
 * @param trnsfSubseqConf
 * @param symbols
 * @param depSymbols
 * @param maxsize
 * @return
 */
size_t encodeTransformSubseq(const paramcabac::TransformedSubSeq& trnsfSubseqConf, util::DataBlock* symbols,
                             util::DataBlock* const depSymbols = nullptr,
                             size_t maxsize = std::numeric_limits<size_t>::max());

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_ENCODE_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
