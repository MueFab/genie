/** @file helpers.cc
 *  @brief This file contains the implementations of the functions defined
 *         in helpers.h.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#include "Common/helpers.h"

#include <time.h>

#include <fstream>

#include "Common/Exceptions.h"
#include "Common/os.h"

namespace calq {

std::string currentDateAndTime(void) {
    // ISO 8601 format: 2007-04-05T14:30:21Z
    char timeString[] = "yyyy-mm-ddTHH:MM:SSZ";

    time_t currentTime = time(NULL);
    if (currentTime == ((time_t)-1)) {
        throwErrorException("time failed");
    }
    struct tm timeinfo;

#ifdef OS_WINDOWS
    errno_t err = gmtime_s(&timeinfo, &currentTime);
    if (err != 0) {
        throwErrorException("gmtime_s failed");
    }
#else
    struct tm *ret = gmtime_r(&currentTime, &timeinfo);
    if (ret == NULL) {
        throwErrorException("gmtime_r failed");
    }
#endif

    if (strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%SZ",
                 &timeinfo) == 0) {
        throwErrorException("strftime failed");
    }

    std::string result(timeString);
    return result;
}

bool fileExists(const std::string &path) {
    if (path.empty() == true) {
        throwErrorException("path is empty");
    }
    std::ifstream ifs(path.c_str());
    return ifs.good();
}

std::string fileBaseName(const std::string &path) {
    if (path.empty() == true) {
        throwErrorException("path is empty");
    }
    std::string const &delims = "/\\";
    return path.substr(path.find_last_of(delims) + 1);
}

std::string fileNameExtension(const std::string &path) {
    if (path.empty() == true) {
        throwErrorException("path is empty");
    }
    if (path.find_last_of(".") != std::string::npos) {
        return path.substr(path.find_last_of(".")+1);
    }
    return "";
}

std::string removeFileNameExtension(const std::string &path) {
    if (path.empty() == true) {
        throwErrorException("path is empty");
    }
    std::string::size_type const p(path.find_last_of('.'));
    return p > 0 && p != std::string::npos ? path.substr(0, p) : path;
}

}  // namespace calq

