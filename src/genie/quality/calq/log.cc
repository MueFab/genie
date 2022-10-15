/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/quality/calq/log.h"

#include <memory>

namespace genie {
namespace quality {
namespace calq {

// -----------------------------------------------------------------------------

static std::unique_ptr<LogConfiguration> currentConfig;

// -----------------------------------------------------------------------------

void setLogging(const LogConfiguration& c) {
    currentConfig = std::unique_ptr<LogConfiguration>(new LogConfiguration(c));
}

// -----------------------------------------------------------------------------

LogConfiguration getLogging() {
    if (!currentConfig) {
        setLogging(loggingPresets::getStandard());
    }
    return *currentConfig;
}

// -----------------------------------------------------------------------------

namespace loggingPresets {

// -----------------------------------------------------------------------------

LogConfiguration getSilent() {
    return {[](const std::string&) {}, [](const std::string&) {}};
}

// -----------------------------------------------------------------------------

LogConfiguration getStandard() {
    return {[](const std::string& msg) { std::cout << msg << std::endl; },
            [](const std::string& msg) { std::cerr << msg << std::endl; }};
}

// -----------------------------------------------------------------------------

}  // namespace loggingPresets

// -----------------------------------------------------------------------------

}  // namespace calq
}  // namespace quality
}  // namespace genie

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
