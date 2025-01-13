/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of utility functions for decoding transformed
 * subsequences.
 *
 * This file contains declarations of utility functions used for decoding
 * subsequences that have undergone various transformations, such as CABAC
 * (Context-Adaptive Binary Arithmetic Coding) transformations and binarization
 * techniques. It also includes helper types and functions for handling the sign
 * flags, binarization parameters, and reading encoded symbols.
 *
 * @details The functions in this file are designed to operate on transformed
 * subsequences, providing functionality for reversing the transformations
 * applied during the encoding process. These transformations include different
 * types of binarization, context selection, and support for bypass mode. The
 * file also defines the `BinFunc` type, which is used to reference specific
 * binarization functions within the `Reader` class.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SUB_SEQ_H_
#define SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SUB_SEQ_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <vector>

#include "genie/entropy/gabac/reader.h"
#include "genie/entropy/paramcabac/transformed_sub_seq.h"

// -----------------------------------------------------------------------------

namespace genie::util {
class DataBlock;  //!< Forward declaration of the DataBlock class for handling
                  //!< data operations.
}

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Type alias for referencing binarization functions within the `Reader`
 * class.
 *
 * This alias is used to specify function pointers to different binarization
 * methods within the `Reader` class, allowing dynamic selection of the
 * appropriate decoding method based on the configuration parameters.
 */
using BinFunc = uint64_t (Reader::*)(const std::vector<unsigned int>&);

// -----------------------------------------------------------------------------

/**
 * @brief Decodes the sign flag for a given symbol value using the specified
 * binarization method.
 *
 * This function reads the sign flag (positive or negative) for a symbol value
 * from the input bitstream using the provided `Reader` object and the specified
 * binarization method (`bin_id`). The sign flag is updated in the
 * `symbol_value` parameter.
 *
 * @param reader Reference to the `Reader` object used for bitstream operations.
 * @param bin_id Binarization method to use for decoding the sign flag.
 * @param symbol_value Reference to the symbol value where the sign flag will be
 * updated.
 */
void DecodeSignFlag(Reader& reader,
                    paramcabac::BinarizationParameters::BinarizationId bin_id,
                    uint64_t& symbol_value);

// -----------------------------------------------------------------------------

/**
 * @brief Retrieves the appropriate binarization function for the given
 * configuration parameters.
 *
 * This function selects the correct binarization function from the `Reader`
 * class based on the specified parameters. The function uses various flags,
 * symbol sizes, and state variables to determine which decoding function to use
 * for the transformed subsequence.
 *
 * @param output_symbol_size Size of the output symbol in bits.
 * @param bypass_flag Indicates if bypass mode is enabled for CABAC decoding.
 * @param bin_id The binarization method identifier.
 * @param binarization_params The parameters for the selected binarization
 * method.
 * @param state_vars State variables used for context modeling.
 * @param bin_params Output vector to store parameters for the selected
 * binarization function.
 * @return A function pointer (`BinFunc`) to the selected binarization method.
 */
BinFunc GetBinarizationReader(
    uint8_t output_symbol_size, bool bypass_flag,
    paramcabac::BinarizationParameters::BinarizationId bin_id,
    const paramcabac::BinarizationParameters& binarization_params,
    const paramcabac::StateVars& state_vars,
    std::vector<unsigned int>& bin_params);

// -----------------------------------------------------------------------------

/**
 * @brief Decodes a transformed subsequence from the bitstream using the
 * specified configuration.
 *
 * This function decodes a subsequence from the input `bitstream` using the
 * given transformation configuration (`trnsf_subseq_conf`). It supports
 * different word sizes, and optionally takes dependent symbols (`dep_symbols`)
 * if higher-order transformations are applied.
 *
 * @param transform_sub_seq_conf Configuration for the transformed subsequence.
 * @param num_encoded_symbols The total number of symbols encoded in the
 * bitstream.
 * @param bitstream Pointer to the input bitstream containing encoded data.
 * @param word_size The Size of each symbol in the bitstream, in bits.
 * @param dep_symbols Pointer to dependent symbols, if any (used for context
 * modeling).
 * @return The number of symbols successfully decoded from the bitstream.
 */
size_t DecodeTransformSubSeq(
    const paramcabac::TransformedSubSeq& transform_sub_seq_conf,
    unsigned int num_encoded_symbols, util::DataBlock* bitstream,
    uint8_t word_size, util::DataBlock* dep_symbols = nullptr);

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SUB_SEQ_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
