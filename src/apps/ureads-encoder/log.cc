#include "log.h"

#include <time.h>

#include "util/exceptions.h"

namespace genie {

std::string currentDateAndTime() {
    // ISO 8601 format: 2007-04-05T14:30:21Z
    char timeString[] = "yyyy-mm-ddTHH:MM:SSZ";

    time_t currentTime = time(nullptr);
    if (currentTime == ((time_t)-1)) {
        UTILS_DIE("time failed");
    }
    struct tm timeinfo {};

#ifdef _WIN32
    errno_t err = gmtime_s(&timeinfo, &currentTime);
    if (err != 0) {
        UTILS_DIE("gmtime_s failed");
    }
#else
    struct tm* ret = gmtime_r(&currentTime, &timeinfo);
    if (ret == nullptr) {
        UTILS_DIE("gmtime_r failed");
    }
#endif

    if (strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%SZ", &timeinfo) == 0) {
        UTILS_DIE("strftime failed");
    }

    std::string result(timeString);
    return result;
}

}  // namespace genie
