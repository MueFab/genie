// Copyright 2018 The genie authors


/**
 *  @file helpers.c
 *  @brief Utility functions implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "common/helpers.h"

#include <string.h>
#include <time.h>

#include "common/os.h"


namespace dsg {


void aprintCurrentDateAndTime(FILE *stream)
{
    // ISO 8601 format: 2007-04-05T14:30:21Z
    char timeString[] = "yyyy-mm-ddTHH:MM:SSZ";

    time_t currentTime = time(NULL);
    if (currentTime == ((time_t) - 1)) {
        return;  // time call failed
    }

    struct tm timeinfo;

#ifdef CABAC_OS_WINDOWS
    errno_t err = gmtime_s(&timeinfo, &currentTime);
    if (err != 0) {
        return;  // gmtime_s call failed
    }
#else
    struct tm *ret = gmtime_r(&currentTime, &timeinfo);
    if (ret == NULL) {
        return;  // gmtime_r call failed
    }
#endif

    if (strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%SZ", &timeinfo) == 0) {
        return;  // strftime call failed
    }

    fprintf(stream, "%s", timeString);
}


}  // namespace dsg
