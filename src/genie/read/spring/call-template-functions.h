/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SPRING_CALL_TEMPLATE_FUNCTIONS_H_
#define SPRING_CALL_TEMPLATE_FUNCTIONS_H_

#include <string>
#include "util.h"
namespace genie {
namespace read {
namespace spring {

void call_reorder(const std::string &temp_dir, compression_params &cp);

void call_encoder(const std::string &temp_dir, compression_params &cp);

}  // namespace spring
}  // namespace read
}  // namespace genie

#endif  // SPRING_CALL_TEMPLATE_FUNCTIONS_H_
