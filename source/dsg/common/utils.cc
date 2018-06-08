// Copyright 2018 The genie authors


/**
 *  @file utils.h
 *  @brief Utilities implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "common/utils.h"

#include <time.h>

#include <fstream>

#include "common/os.h"


namespace dsg {


std::string dateTime(void)
{
    // ISO 8601 format: 2007-04-05T14:30:21Z
    char timeString[] = "yyyy-mm-ddTHH:MM:SSZ";

    time_t currentTime = time(NULL);
    if (currentTime == ((time_t)-1)) {
        throw std::runtime_error("call to time() failed");
    }
    struct tm timeinfo;

#ifdef OS_WINDOWS
    errno_t err = gmtime_s(&timeinfo, &currentTime);
    if (err != 0) {
        std::runtime_error("call to gmtime_s() failed");
    }
#else
    struct tm *ret = gmtime_r(&currentTime, &timeinfo);
    if (ret == NULL) {
        std::runtime_error("call to gmtime_r() failed");
    }
#endif

    if (strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%SZ",
                 &timeinfo) == 0) {
        std::runtime_error("call to strftime() failed");
    }

    std::string result(timeString);
    return result;
}


bool fileExists(const std::string& path)
{
    std::ifstream ifs(path.c_str());
    return ifs.good();
}


}  // namespace dsg
