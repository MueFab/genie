/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_DECODE_TRANSFORMED_SUBSEQ_H_
#define GABAC_DECODE_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>

#include <genie/entropy/paramcabac/transformed-subseq.h>
#include "reader.h"

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

// ---------------------------------------------------------------------------------------------------------------------

/**
 *
 * @param
 */
typedef uint64_t (Reader::*binFunc)(const std::vector<unsigned int>);

// ---------------------------------------------------------------------------------------------------------------------

/**
 *
 * @param
 */
inline void decodeSignFlag(Reader &reader, const paramcabac::BinarizationParameters::BinarizationId binID,
                           uint64_t &symbolValue);


// ---------------------------------------------------------------------------------------------------------------------

/**
 *
 * @param
 */
inline binFunc getBinarizor(const uint8_t outputSymbolSize, const bool bypassFlag,
                            const paramcabac::BinarizationParameters::BinarizationId binID,
                            const paramcabac::BinarizationParameters &binarzationParams,
                            const paramcabac::StateVars &stateVars, std::vector<unsigned int> &binParams);

// ---------------------------------------------------------------------------------------------------------------------

/**
 *
 * @param trnsfSubseqConf
 * @param numEncodedSymbols
 * @param bitstream
 * @param depSymbols
 * @return
 */
size_t decodeTransformSubseq(const paramcabac::TransformedSubSeq& trnsfSubseqConf, const unsigned int numEncodedSymbols,
                             util::DataBlock* bitstream, util::DataBlock* const depSymbols = nullptr);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GABAC_DECODE_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------