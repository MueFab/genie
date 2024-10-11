/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of utility functions for decoding transformed subsequences.
 *
 * This file contains declarations of utility functions used for decoding subsequences that
 * have undergone various transformations, such as CABAC (Context-Adaptive Binary Arithmetic Coding)
 * transformations and binarization techniques. It also includes helper types and functions for handling
 * the sign flags, binarization parameters, and reading encoded symbols.
 *
 * @details The functions in this file are designed to operate on transformed subsequences,
 * providing functionality for reversing the transformations applied during the encoding process.
 * These transformations include different types of binarization, context selection, and
 * support for bypass mode. The file also defines the `binFunc` type, which is used to
 * reference specific binarization functions within the `Reader` class.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SUBSEQ_H_
#define SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>                                        // For standard integer types (e.g., uint64_t).
#include <vector>                                         // For using std::vector in function parameters.
#include "genie/entropy/gabac/reader.h"                   // For accessing the Reader class and its methods.
#include "genie/entropy/paramcabac/transformed-subseq.h"  // For transformed subsequence configurations.

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {
class DataBlock;  //!< Forward declaration of the DataBlock class for handling data operations.
}

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Type alias for referencing binarization functions within the `Reader` class.
 *
 * This alias is used to specify function pointers to different binarization methods
 * within the `Reader` class, allowing dynamic selection of the appropriate decoding
 * method based on the configuration parameters.
 */
typedef uint64_t (Reader::*binFunc)(const std::vector<unsigned int>&);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Decodes the sign flag for a given symbol value using the specified binarization method.
 *
 * This function reads the sign flag (positive or negative) for a symbol value from the input
 * bitstream using the provided `Reader` object and the specified binarization method (`binID`).
 * The sign flag is updated in the `symbolValue` parameter.
 *
 * @param reader Reference to the `Reader` object used for bitstream operations.
 * @param binID Binarization method to use for decoding the sign flag.
 * @param symbolValue Reference to the symbol value where the sign flag will be updated.
 */
void decodeSignFlag(Reader& reader, paramcabac::BinarizationParameters::BinarizationId binID, uint64_t& symbolValue);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Retrieves the appropriate binarization function for the given configuration parameters.
 *
 * This function selects the correct binarization function from the `Reader` class based on the
 * specified parameters. The function uses various flags, symbol sizes, and state variables to
 * determine which decoding function to use for the transformed subsequence.
 *
 * @param outputSymbolSize Size of the output symbol in bits.
 * @param bypassFlag Indicates if bypass mode is enabled for CABAC decoding.
 * @param binID The binarization method identifier.
 * @param binarzationParams The parameters for the selected binarization method.
 * @param stateVars State variables used for context modeling.
 * @param binParams Output vector to store parameters for the selected binarization function.
 * @return A function pointer (`binFunc`) to the selected binarization method.
 */
binFunc getBinarizorReader(uint8_t outputSymbolSize, bool bypassFlag,
                           paramcabac::BinarizationParameters::BinarizationId binID,
                           const paramcabac::BinarizationParameters& binarzationParams,
                           const paramcabac::StateVars& stateVars, std::vector<unsigned int>& binParams);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Decodes a transformed subsequence from the bitstream using the specified configuration.
 *
 * This function decodes a subsequence from the input `bitstream` using the given transformation
 * configuration (`trnsfSubseqConf`). It supports different word sizes, and optionally takes
 * dependent symbols (`depSymbols`) if higher-order transformations are applied.
 *
 * @param trnsfSubseqConf Configuration for the transformed subsequence.
 * @param numEncodedSymbols The total number of symbols encoded in the bitstream.
 * @param bitstream Pointer to the input bitstream containing encoded data.
 * @param wordsize The size of each symbol in the bitstream, in bits.
 * @param depSymbols Pointer to dependent symbols, if any (used for context modeling).
 * @return The number of symbols successfully decoded from the bitstream.
 */
size_t decodeTransformSubseq(const paramcabac::TransformedSubSeq& trnsfSubseqConf, unsigned int numEncodedSymbols,
                             util::DataBlock* bitstream, uint8_t wordsize, util::DataBlock* depSymbols = nullptr);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
