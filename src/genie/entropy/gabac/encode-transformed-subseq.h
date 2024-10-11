/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the function for encoding transformed subsequences in GABAC.
 *
 * This file contains the declaration of the `encodeTransformSubseq` function, which performs the
 * GABAC encoding of a transformed subsequence. The function utilizes configurations defined in
 * `TransformedSubSeq` and takes symbol data and optional dependency symbols to generate
 * a compressed representation.
 *
 * @details The `encodeTransformSubseq` function encodes transformed subsequences using
 * specified configurations. It supports processing both independent and dependent symbols,
 * making it suitable for a wide range of genomic data transformations. The encoded output
 * is stored in the provided `DataBlock` objects, and the function can limit the encoding
 * process based on a specified maximum size.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_ENCODE_TRANSFORMED_SUBSEQ_H_
#define SRC_GENIE_ENTROPY_GABAC_ENCODE_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------

#define NOMINMAX
#include <cstddef>                //!< Include for standard size types.
#include <cstdint>                //!< Include for standard integer types.
#include <limits>                 //!< Include for defining size limits.
#include <vector>                 //!< Include for handling symbol vectors.
#include "genie/entropy/paramcabac/transformed-subseq.h"  //!< Include for transformation configuration definitions.

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Forward declaration of the DataBlock class.
 *
 * The `DataBlock` class is used to manage and manipulate blocks of binary data
 * during the encoding and decoding process. This forward declaration allows the
 * `encodeTransformSubseq` function to utilize `DataBlock` pointers without needing
 * the complete class definition at this point.
 */
class DataBlock;

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Encodes a transformed subsequence using GABAC configurations.
 *
 * This function performs the encoding of a transformed subsequence using the specified
 * configuration. The input symbols are processed according to the transformation configuration
 * (`trnsfSubseqConf`) and stored in the provided `symbols` `DataBlock`. If the subsequence
 * is dependent on other symbols (e.g., for reference-based compression), these can be passed
 * through the optional `depSymbols` parameter.
 *
 * @param trnsfSubseqConf Configuration settings for the transformed subsequence.
 * @param symbols Pointer to the `DataBlock` containing the symbols to be encoded.
 * @param depSymbols Optional pointer to the `DataBlock` containing dependent symbols.
 * @param maxsize Maximum size limit for the encoded output. The default is set to the maximum size value.
 * @return The number of encoded symbols processed during the transformation.
 *
 * @details The function encodes the symbols based on the `TransformedSubSeq` settings, applying
 * the appropriate transformations and compression methods. It returns the number of symbols
 * successfully encoded. The `maxsize` parameter is used to control the amount of data encoded,
 * allowing for partial encoding in scenarios where memory or size constraints are a concern.
 */
size_t encodeTransformSubseq(const paramcabac::TransformedSubSeq& trnsfSubseqConf, util::DataBlock* symbols,
                             util::DataBlock* depSymbols = nullptr,
                             size_t maxsize = std::numeric_limits<size_t>::max());

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_ENCODE_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
