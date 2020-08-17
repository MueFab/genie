/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SPRING_GENERATE_READ_STREAMS_H_
#define SPRING_GENERATE_READ_STREAMS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>

#include <genie/core/read-encoder.h>
#include "util.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

/**
 *
 * @param temp_dir
 * @param cp
 * @param entropycoder
 * @param stats
 */
void generate_read_streams(const std::string &temp_dir, const compression_params &cp,
                           core::ReadEncoder::EntropySelector *entropycoder,
                           std::vector<core::parameter::ParameterSet> &, core::stats::PerfStats &stats);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SPRING_GENERATE_READ_STREAMS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------