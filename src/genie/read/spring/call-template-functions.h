/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_CALL_TEMPLATE_FUNCTIONS_H_
#define SRC_GENIE_READ_SPRING_CALL_TEMPLATE_FUNCTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/read/spring/util.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

/**
 * @brief
 * @param temp_dir
 * @param cp
 */
void call_reorder(const std::string &temp_dir, compression_params &cp);

/**
 * @brief
 * @param temp_dir
 * @param cp
 */
void call_encoder(const std::string &temp_dir, compression_params &cp);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_CALL_TEMPLATE_FUNCTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
