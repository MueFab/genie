/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SUBSEQ_H_
#define SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "genie/entropy/gabac/reader.h"
#include "genie/entropy/paramcabac/transformed-subseq.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {
class DataBlock;
}

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief
 */
typedef uint64_t (Reader:: *binFunc)(const std::vector<unsigned int> &);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief
 * @param reader
 * @param binID
 * @param symbolValue
 */
void decodeSignFlag(Reader &reader, paramcabac::BinarizationParameters::BinarizationId binID, uint64_t &symbolValue);

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
binFunc getBinarizorReader(uint8_t outputSymbolSize, bool bypassFlag,
                           paramcabac::BinarizationParameters::BinarizationId binID,
                           const paramcabac::BinarizationParameters &binarzationParams,
                           const paramcabac::StateVars &stateVars, std::vector<unsigned int> &binParams);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief
 * @param trnsfSubseqConf
 * @param numEncodedSymbols
 * @param bitstream
 * @param wordsize
 * @param depSymbols
 * @return
 */
size_t decodeTransformSubseq(const paramcabac::TransformedSubSeq &trnsfSubseqConf, unsigned int numEncodedSymbols,
                             util::DataBlock *bitstream, uint8_t wordsize, util::DataBlock *depSymbols = nullptr);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
