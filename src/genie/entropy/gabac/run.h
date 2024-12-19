/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 * @brief Declaration of the `run` function for starting the gabac core
 * processing.
 *
 * This file provides the declaration of the `run` function used to initiate the
 * gabac core operations. The `run` function is responsible for configuring and
 * executing either encoding or decoding based on the provided settings. It
 * utilizes the specified `IOConfiguration` and `EncodingConfiguration` to
 * control the process.
 *
 * @details The `run` function is a central entry point for the gabac core
 * operations, allowing for both encoding and decoding. The function accepts
 * configurations for I/O and encoding, along with a flag indicating the
 * operation mode (encode/Decode). Depending on the provided configuration, it
 * either encodes or decodes the data accordingly.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_RUN_H_
#define SRC_GENIE_ENTROPY_GABAC_RUN_H_

// -----------------------------------------------------------------------------

#include <cstdint>

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// Forward declarations of configuration structures
class EncodingConfiguration;  //!< @brief Configuration for the gabac encoding.
struct IoConfiguration;       //!< @brief Configuration for the I/O operations.

/**
 * @brief Executes a run of the gabac core process based on the provided
 * configurations.
 *
 * This function initiates the gabac core processing using the specified I/O and
 * encoding configurations. The `run` function serves as the entry point for
 * performing either encoding or decoding, determined by the `Decode` parameter.
 *
 * @param conf The I/O configuration that controls the input and output streams.
 * @param en_conf The gabac encoding configuration that defines the encoding
 * parameters.
 * @param decode If set to `true`, the function performs decoding. If set to
 * `false`, it performs encoding.
 * @return The number of symbols processed during the operation.
 */
uint64_t Run(const IoConfiguration& conf, const EncodingConfiguration& en_conf,
             bool decode);

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_RUN_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
