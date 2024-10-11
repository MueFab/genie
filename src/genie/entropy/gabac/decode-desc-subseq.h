/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of functions for decoding descriptor subsequences in the GABAC framework.
 *
 * This file contains the declarations of utility functions used for decoding descriptor subsequences
 * that have undergone transformation coding using the GABAC library. The decoding process involves
 * reversing transformations such as run-length encoding, context-based arithmetic coding, and other
 * parameter-based transformations defined in the `paramcabac::Subsequence` configuration.
 *
 * @details The functions provided in this file operate on encoded subsequences, allowing the extraction
 * of raw data from transformed streams. They support different configurations and transformations,
 * making them versatile for use with various genomic data formats. The main functions are designed to
 * handle the specific decoding and transformation needs for descriptor subsequences, which are a critical
 * part of the MPEG-G standard for genomic data compression.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_DECODE_DESC_SUBSEQ_H_
#define SRC_GENIE_ENTROPY_GABAC_DECODE_DESC_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>                                 // For standard integer types (e.g., uint64_t).
#include <vector>                                  // For using std::vector in function parameters.
#include "genie/entropy/paramcabac/subsequence.h"  // For the subsequence transformation configuration.
#include "genie/util/data-block.h"                 // For the DataBlock class, which handles raw data.

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingConfiguration;  // Forward declaration of the EncodingConfiguration structure.
struct IOConfiguration;        // Forward declaration of the IOConfiguration structure.

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Performs the inverse transformation on a given subsequence.
 *
 * This function reverses the transformations applied to a subsequence during encoding.
 * It takes a configuration (`subseqCfg`) that defines the type of transformations applied
 * and a vector of data blocks (`transformedSubseqs`) representing the transformed symbols.
 * The function modifies the `transformedSubseqs` to hold the decoded symbols.
 *
 * @param subseqCfg The configuration describing the applied transformations on the subsequence.
 * @param transformedSubseqs Vector of transformed data blocks to be decoded.
 */
void doInverseSubsequenceTransform(const paramcabac::Subsequence& subseqCfg,
                                   std::vector<util::DataBlock>* transformedSubseqs);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Decodes a descriptor subsequence using the given input and encoding configurations.
 *
 * This function decodes a subsequence of encoded genomic data according to the specified
 * `IOConfiguration` and `EncodingConfiguration` settings. It reads the encoded data, applies
 * the necessary transformations in reverse, and reconstructs the original subsequence.
 *
 * @param ioConf Configuration for input and output data handling (I/O).
 * @param enConf Configuration for encoding/decoding parameters and transformations.
 * @return The total number of symbols decoded from the subsequence.
 */
uint64_t decodeDescSubsequence(const IOConfiguration& ioConf, const EncodingConfiguration& enConf);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_DECODE_DESC_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
