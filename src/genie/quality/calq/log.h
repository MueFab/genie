/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_LOG_H_
#define SRC_GENIE_QUALITY_CALQ_LOG_H_

// -----------------------------------------------------------------------------

#include <functional>
#include <iostream>
#include <string>

// -----------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace calq {

/**
 * Output streams for logging
 */
struct LogConfiguration {
    /**
     * Output to stdout
     */
    std::function<void(const std::string&)> standardOut;

    /**
     * Output to stderr
     */
    std::function<void(const std::string&)> errorOut;
};

/**
 * Redirect logging streams
 * @param c New configuration
 */
void setLogging(const LogConfiguration& c);

/**
 * @return Current logging configuration
 */
LogConfiguration getLogging();

// -----------------------------------------------------------------------------

/**
 * Contains some handy default configurations
 */
namespace loggingPresets {

// -----------------------------------------------------------------------------

/**
 * @return Configuration not logging at all
 */
LogConfiguration getSilent();

/**
 * @return Using std::cout and std::cerr
 */
LogConfiguration getStandard();

// -----------------------------------------------------------------------------

}  // namespace loggingPresets

// -----------------------------------------------------------------------------

}  // namespace calq
}  // namespace quality
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_LOG_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
