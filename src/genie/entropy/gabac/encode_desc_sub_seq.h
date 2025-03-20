/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the encoding functions for descriptor subsequences in
 * the GABAC module.
 *
 * This file contains the declarations of functions used for encoding descriptor
 * subsequences in the GABAC (Genomic Arithmetic Binary Adaptive Coding) module.
 * The main functions declared here are responsible for performing
 * transformations and encoding processes on descriptor subsequences using the
 * configurations specified in the `IOConfiguration` and `EncodingConfiguration`
 * structures.
 *
 * @details The `DoSubsequenceTransform` function applies a transformation on
 * the provided subsequence data based on the configurations defined in the
 * `paramcabac::Subsequence` structure. The primary function,
 * `EncodeDescSubsequence`, handles the entire encoding process for a descriptor
 * subsequence, utilizing the GABAC library to perform CABAC-based compression
 * on genomic descriptor subsequences.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_ENCODE_DESC_SUB_SEQ_H_
#define SRC_GENIE_ENTROPY_GABAC_ENCODE_DESC_SUB_SEQ_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <vector>

#include "genie/entropy/paramcabac/subsequence.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------

/**
 * @brief Configuration structure for I/O settings used during GABAC encoding.
 *
 * This structure contains all necessary parameters and settings required to
 * manage input and output configurations for the GABAC encoding process. It
 * includes file paths, buffer settings, and any other relevant I/O management
 * variables.
 */
struct IoConfiguration;

/**
 * @brief Configuration structure for GABAC encoding settings.
 *
 * This structure holds all encoding-specific configurations for the GABAC
 * compression process. It defines the transformations, binarization settings,
 * context models, and other parameters used during the CABAC-based encoding of
 * descriptor subsequences.
 */
struct EncodingConfiguration;

/**
 * @brief Performs the subsequence transformation as defined in the given
 * configuration.
 *
 * This function applies the transformation on the provided subsequence data
 * based on the configurations specified in the `subseq_cfg_` parameter. It
 * transforms the input `transformed_subseqs` data using operations such as
 * run-length encoding (RLE), merge coding, match coding, and others.
 *
 * @param sub_seq_cfg Configuration settings for the subsequence transformation.
 * @param transformed_sub_seqs Pointer to a vector of `util::DataBlock` objects
 * containing the transformed subsequences.
 *
 * @details This function modifies the `transformed_subseqs` based on the
 * configuration in `subseq_cfg_`, applying a transformation algorithm to each
 * subsequence as specified in the MPEG-G configuration.
 */
void DoSubsequenceTransform(const paramcabac::Subsequence& sub_seq_cfg,
                            std::vector<util::DataBlock>* transformed_sub_seqs);

/**
 * @brief Encodes a descriptor subsequence using the specified configurations.
 *
 * This function encodes a given descriptor subsequence using the configurations
 * defined in the `IOConfiguration` and `EncodingConfiguration` structures. It
 * performs the necessary transformations, CABAC compression, and other encoding
 * steps required to generate a compressed subsequence.
 *
 * @param conf The input/output configuration settings for the GABAC encoding
 * process.
 * @param en_conf The encoding configuration settings defining how to encode the
 * descriptor subsequence.
 * @return The Size of the encoded subsequence in bytes.
 *
 * @details This function is the main interface for encoding descriptor
 * subsequences in the GABAC module. It coordinates the various encoding stages,
 * applies transformations, and compresses the subsequence data using
 * CABAC-based encoding schemes. The function returns the Size of the encoded
 * subsequence after compression.
 */
uint64_t EncodeDescSubsequence(const IoConfiguration& conf,
                               const EncodingConfiguration& en_conf);

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_ENCODE_DESC_SUB_SEQ_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------