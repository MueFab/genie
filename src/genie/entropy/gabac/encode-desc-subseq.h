/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the encoding functions for descriptor subsequences in the GABAC module.
 *
 * This file contains the declarations of functions used for encoding descriptor subsequences in the GABAC (Genomic
 * Arithmetic Binary Adaptive Coding) module. The main functions declared here are responsible for performing
 * transformations and encoding processes on descriptor subsequences using the configurations specified in the
 * `IOConfiguration` and `EncodingConfiguration` structures.
 *
 * @details The `doSubsequenceTransform` function applies a transformation on the provided subsequence data based on
 * the configurations defined in the `paramcabac::Subsequence` structure. The primary function, `encodeDescSubsequence`,
 * handles the entire encoding process for a descriptor subsequence, utilizing the GABAC library to perform CABAC-based
 * compression on genomic descriptor subsequences.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_ENCODE_DESC_SUBSEQ_H_
#define SRC_GENIE_ENTROPY_GABAC_ENCODE_DESC_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>  // For uint64_t, uint8_t, and other fixed-width integer types.
#include <vector>   // For using the std::vector container.
#include "genie/entropy/paramcabac/subsequence.h"  // For paramcabac::Subsequence definition.
#include "genie/util/data-block.h"                 // For util::DataBlock class definition.

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Configuration structure for I/O settings used during GABAC encoding.
 *
 * This structure contains all necessary parameters and settings required to manage input and output
 * configurations for the GABAC encoding process. It includes file paths, buffer settings, and any
 * other relevant I/O management variables.
 */
struct IOConfiguration;

/**
 * @brief Configuration structure for GABAC encoding settings.
 *
 * This structure holds all encoding-specific configurations for the GABAC compression process.
 * It defines the transformations, binarization settings, context models, and other parameters
 * used during the CABAC-based encoding of descriptor subsequences.
 */
struct EncodingConfiguration;

/**
 * @brief Performs the subsequence transformation as defined in the given configuration.
 *
 * This function applies the transformation on the provided subsequence data based on the
 * configurations specified in the `subseqCfg` parameter. It transforms the input `transformedSubseqs`
 * data using operations such as run-length encoding (RLE), merge coding, match coding, and others.
 *
 * @param subseqCfg Configuration settings for the subsequence transformation.
 * @param transformedSubseqs Pointer to a vector of `util::DataBlock` objects containing the transformed subsequences.
 *
 * @details This function modifies the `transformedSubseqs` based on the configuration in `subseqCfg`,
 * applying a transformation algorithm to each subsequence as specified in the MPEG-G configuration.
 */
void doSubsequenceTransform(const paramcabac::Subsequence& subseqCfg, std::vector<util::DataBlock>* transformedSubseqs);

/**
 * @brief Encodes a descriptor subsequence using the specified configurations.
 *
 * This function encodes a given descriptor subsequence using the configurations defined in the
 * `IOConfiguration` and `EncodingConfiguration` structures. It performs the necessary transformations,
 * CABAC compression, and other encoding steps required to generate a compressed subsequence.
 *
 * @param conf The input/output configuration settings for the GABAC encoding process.
 * @param enConf The encoding configuration settings defining how to encode the descriptor subsequence.
 * @return The size of the encoded subsequence in bytes.
 *
 * @details This function is the main interface for encoding descriptor subsequences in the GABAC module.
 * It coordinates the various encoding stages, applies transformations, and compresses the subsequence data
 * using CABAC-based encoding schemes. The function returns the size of the encoded subsequence after compression.
 */
uint64_t encodeDescSubsequence(const IOConfiguration& conf, const EncodingConfiguration& enConf);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_ENCODE_DESC_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
