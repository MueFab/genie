/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SPRING_SPRING_H_
#define SPRING_SPRING_H_

#include <map>
#include <string>
#include <vector>

#include <genie/generation.h>
#include <genie/stream-saver.h>
#include <genie/core/stats/perf-stats.h>

#include "return-structures.h"
#include "util.h"
namespace genie {
namespace read {
namespace spring {

void call_reorder(const std::string &temp_dir, compression_params &cp);

void call_encoder(const std::string &temp_dir, compression_params &cp);

std::string random_string(size_t length);

}  // namespace spring
}  // namespace read
}  // namespace genie
#endif  // SPRING_SPRING_H_
