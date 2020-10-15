/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_ENCODE_TRANSFORMED_SUBSEQ_H_
#define GABAC_ENCODE_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

#include <genie/entropy/paramcabac/transformed-subseq.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 *
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

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GABAC_ENCODE_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------