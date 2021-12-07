/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_GENERATE_READ_STREAMS_H_
#define SRC_GENIE_READ_SPRING_GENERATE_READ_STREAMS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>
#include <vector>
#include "genie/core/read-encoder.h"
#include "genie/read/spring/util.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

/**
 * @brief
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

#endif  // SRC_GENIE_READ_SPRING_GENERATE_READ_STREAMS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
