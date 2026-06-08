/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the function for encoding transformed subsequences in
 * GABAC.
 *
 * This file contains the declaration of the `EncodeTransformSubseq` function,
 * which performs the GABAC encoding of a transformed subsequence. The function
 * utilizes configurations defined in `TransformedSubSeq` and takes symbol data
 * and optional dependency symbols to generate a compressed representation.
 *
 * @details The `EncodeTransformSubseq` function encodes transformed
 * subsequences using specified configurations. It supports processing both
 * independent and dependent symbols, making it suitable for a wide range of
 * genomic data transformations. The encoded output is stored in the provided
 * `DataBlock` objects, and the function can limit the encoding process based on
 * a specified maximum Size.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_ENCODE_TRANSFORMED_SUB_SEQ_H_
#define SRC_GENIE_ENTROPY_GABAC_ENCODE_TRANSFORMED_SUB_SEQ_H_

// -----------------------------------------------------------------------------

#define NOMINMAX
#include <cstddef>
#include <cstdint>
#include <limits>

#include "genie/entropy/paramcabac/transformed_sub_seq.h"

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Forward declaration of the DataBlock class.
 *
 * The `DataBlock` class is used to manage and manipulate blocks of binary data
 * during the encoding and decoding process. This forward declaration allows the
 * `EncodeTransformSubseq` function to utilize `DataBlock` pointers without
 * needing the complete class definition at this point.
 */
class DataBlock;

}  // namespace genie::util

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Encodes a transformed subsequence using GABAC configurations.
 *
 * This function performs the encoding of a transformed subsequence using the
 * specified configuration. The input symbols are processed according to the
 * transformation configuration
 * (`trnsf_subseq_conf`) and stored in the provided `symbols` `DataBlock`. If
 * the subsequence is dependent on other symbols (e.g., for reference-based
 * compression), these can be passed through the optional `dep_symbols`
 * parameter.
 *
 * @param transform_sub_seq_conf Configuration settings for the transformed
 * subsequence.
 * @param symbols Pointer to the `DataBlock` containing the symbols to be
 * encoded.
 * @param dep_symbols Optional pointer to the `DataBlock` containing dependent
 * symbols.
 * @param max_size Maximum size limit for the encoded output. The default is set
 * to the maximum Size value.
 * @return The number of encoded symbols processed during the transformation.
 *
 * @details The function encodes the symbols based on the `TransformedSubSeq`
 * settings, applying the appropriate transformations and compression methods.
 * It returns the number of symbols successfully encoded. The `maxsize`
 * parameter is used to control the amount of data encoded, allowing for partial
 * encoding in scenarios where memory or Size constraints are a concern.
 */
size_t EncodeTransformSubSeq(
    const paramcabac::TransformedSubSeq& transform_sub_seq_conf,
    util::DataBlock* symbols, util::DataBlock* dep_symbols = nullptr,
    size_t max_size = std::numeric_limits<size_t>::max());

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_ENCODE_TRANSFORMED_SUB_SEQ_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
