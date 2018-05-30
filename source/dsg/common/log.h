// Copyright 2018 The genie authors


/**
 *  @file log.h
 *  @brief Log macros
 *  @author Jan Voges
 *  @bug No known bugs
 */


#ifndef DSG_COMMON_LOG_H_
#define DSG_COMMON_LOG_H_


#include <stdio.h>
#include <string.h>

#include "common/helpers.h"


/**
 *  @brief Log macro
 */
#ifdef DSG_ENABLE_LOG
    #define DSG_LOG(c, ...) \
        do { \
            fflush(stdout); \
            fflush(stderr); \
            fprintf(stderr, "[DSG] "); \
            ac_printCurrentDateAndTime(stderr); \
            char *file = strstr(__FILE__, "ac/src/ac/"); \
            if (file != NULL) { fprintf(stderr, " %s:", (file + 10)); } \
            fprintf(stderr, "%s:", __FUNCTION__); \
            fprintf(stderr, "%d: ", __LINE__); \
            fprintf(stderr, ""c"", ##__VA_ARGS__); \
        } while (0)
#else
    #define DSG_LOG(c, ...) do { } while (0)
#endif


#endif  // DSG_COMMON_LOG_H_
