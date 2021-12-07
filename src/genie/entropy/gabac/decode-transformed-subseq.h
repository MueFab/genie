/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SUBSEQ_H_
#define SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "genie/entropy/gabac/reader.h"
#include "genie/entropy/paramcabac/transformed-subseq.h"

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
 * @brief
 */
typedef uint64_t (Reader::*binFunc)(const std::vector<unsigned int>);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief
 * @param reader
 * @param binID
 * @param symbolValue
 */
void decodeSignFlag(Reader &reader, const paramcabac::BinarizationParameters::BinarizationId binID,
                    uint64_t &symbolValue);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief
 * @param outputSymbolSize
 * @param bypassFlag
 * @param binID
 * @param binarzationParams
 * @param stateVars
 * @param binParams
 * @return
 */
binFunc getBinarizorReader(const uint8_t outputSymbolSize, const bool bypassFlag,
                           const paramcabac::BinarizationParameters::BinarizationId binID,
                           const paramcabac::BinarizationParameters &binarzationParams,
                           const paramcabac::StateVars &stateVars, std::vector<unsigned int> &binParams);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief
 * @param trnsfSubseqConf
 * @param numEncodedSymbols
 * @param bitstream
 * @param depSymbols
 * @return
 */
size_t decodeTransformSubseq(const paramcabac::TransformedSubSeq &trnsfSubseqConf, const unsigned int numEncodedSymbols,
                             util::DataBlock *bitstream, util::DataBlock *const depSymbols = nullptr);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
