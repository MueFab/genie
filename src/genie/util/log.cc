/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "log.h"
#include <ctime>
#include "exceptions.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

std::string currentDateAndTime() {
    // ISO 8601 format: 2007-04-05T14:30:21Z
    char timeString[] = "yyyy-mm-ddTHH:MM:SSZ";

    time_t currentTime = time(nullptr);
    if (currentTime == ((time_t)-1)) {
        DIE("Getting the current time failed");
    }
    struct tm timeInfo {};

#ifdef _WIN32
    errno_t rc = gmtime_s(&timeInfo, &currentTime);
    if (rc != 0) {
        DIE("Conversion to UTC failed failed");
    }
#else
    struct tm *rc = gmtime_r(&currentTime, &timeInfo);
    if (rc == nullptr) {
        DIE("Conversion to UTC failed failed");
    }
#endif

    if (strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%SZ", &timeInfo) == 0) {
        DIE("Formatting the time failed");
    }

    std::string result(timeString);
    return result;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------