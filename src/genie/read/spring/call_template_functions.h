/**
 * Copyright 2018-2024 The Genie Authors.
 * @file call_template_functions.h
 * @brief Header file for function templates used in the Spring module of Genie
 *
 * This file defines the function templates `call_reorder` and `call_encoder`,
 * which are used to invoke specific encoding and reordering operations
 * in the Spring module based on the compression parameters.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_CALL_TEMPLATE_FUNCTIONS_H_
#define SRC_GENIE_READ_SPRING_CALL_TEMPLATE_FUNCTIONS_H_

// -----------------------------------------------------------------------------

#include <string>

#include "genie/read/spring/util.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

/**
 * @brief Invokes the reorder function for the Spring module.
 *
 * This function is responsible for calling the reordering process based on
 * the specified compression parameters and temporary directory path. It sets
 * up the necessary environment and configurations to handle read reordering.
 *
 * @param temp_dir Path to the temporary directory used during processing.
 * @param cp Compression parameters defining the properties and settings for
 * reordering.
 */
void CallReorder(const std::string& temp_dir, const CompressionParams& cp);

/**
 * @brief Invokes the encoder function for the Spring module.
 *
 * This function is responsible for calling the encoding process based on
 * the specified compression parameters and temporary directory path. It sets
 * up the necessary environment and configurations to handle read encoding.
 *
 * @param temp_dir Path to the temporary directory used during processing.
 * @param cp Compression parameters defining the properties and settings for
 * encoding.
 */
void CallEncoder(const std::string& temp_dir, const CompressionParams& cp);

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_CALL_TEMPLATE_FUNCTIONS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
