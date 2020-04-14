/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SPRING_COMBINE_AUS_H_
#define SPRING_COMBINE_AUS_H_

#include <genie/core/stats/perf-stats.h>
#include <string>
#include "util.h"

namespace genie {
namespace read {
namespace spring {

void combine_aus(const std::string &temp_dir, compression_params &cp,
                 const std::vector<std::vector<entropy::gabac::EncodingConfiguration>> &configs,
                 const std::string &outputFilePath, genie::core::stats::FastqStats *stats);

}  // namespace spring
}  // namespace read
}  // namespace genie

#endif  // SPRING_COMBINE_AUS_H_
