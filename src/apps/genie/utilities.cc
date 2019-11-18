// Copyright 2018 The genie authors

/**
 *  @file utilitiess.h
 *  @brief Utilities implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */

#include "utilities.h"

#include <time.h>

#include <sys/stat.h>
#include <fstream>

#include "operating-system.h"
#include "util/exceptions.h"

namespace dsg {
namespace common {

std::string dateTime(void) {
    // ISO 8601 format: 2007-04-05T14:30:21Z
    char timeString[] = "yyyy-mm-ddTHH:MM:SSZ";

    time_t currentTime = time(NULL);
    if (currentTime == ((time_t)-1)) {
        UTILS_DIE("call to time() failed");
    }
    struct tm timeinfo;

#ifdef OS_WINDOWS
    errno_t err = gmtime_s(&timeinfo, &currentTime);
    if (err != 0) {
        UTILS_DIE("call to gmtime_s() failed");
    }
#else
    struct tm* ret = gmtime_r(&currentTime, &timeinfo);
    if (ret == NULL) {
        UTILS_DIE("call to gmtime_r() failed");
    }
#endif

    if (strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%SZ", &timeinfo) == 0) {
        UTILS_DIE("call to strftime() failed");
    }

    std::string result(timeString);
    return result;
}

std::string fileBaseName(const std::string& path) {
    if (path.empty()) {
        UTILS_DIE("path is empty");
    }

    std::string delimiters = "/\\";

    return path.substr(path.find_last_of(delimiters) + 1);
}

bool fileExists(const std::string& path) {
    if (path.empty()) {
        UTILS_DIE("path is empty");
    }

    std::ifstream ifs(path.c_str());

    return ifs.good();
}

}  // namespace common
}  // namespace dsg
