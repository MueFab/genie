/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef UTIL_LOG_H_
#define UTIL_LOG_H_

#include <iostream>
#include <string>

#define LOG_TRACE UTIL_LOG_TRACE
#define LOG_DEBUG UTIL_LOG_DEBUG
#define LOG_INFO UTIL_LOG_INFO
#define LOG_WARNING UTIL_LOG_WARNING
#define LOG_ERROR UTIL_LOG_ERROR
#define LOG_FATAL UTIL_LOG_FATAL

#define UTIL_LOG_TRACE \
    (util::LazyLineBreakAndFlushStdout(), std::cout << "[" << util::currentDateAndTime() << "] [trace] ")
#define UTIL_LOG_DEBUG \
    (util::LazyLineBreakAndFlushStdout(), std::cout << "[" << util::currentDateAndTime() << "] [debug] ")
#define UTIL_LOG_INFO \
    (util::LazyLineBreakAndFlushStdout(), std::cout << "[" << util::currentDateAndTime() << "] [info] ")
#define UTIL_LOG_WARNING \
    (util::LazyLineBreakAndFlushStdout(), std::cout << "[" << util::currentDateAndTime() << "] [warning] ")
#define UTIL_LOG_ERROR \
    (util::LazyLineBreakAndFlushStderr(), std::cerr << "[" << util::currentDateAndTime() << "] [error] ")
#define UTIL_LOG_FATAL \
    (util::LazyLineBreakAndFlushStderr(), std::cerr << "[" << util::currentDateAndTime() << "] [fatal] ")

namespace util {

struct LazyLineBreakAndFlushStdout {
    ~LazyLineBreakAndFlushStdout() { std::cout << std::endl; }  // this also flushes stdout
};

struct LazyLineBreakAndFlushStderr {
    ~LazyLineBreakAndFlushStderr() { std::cerr << std::endl; }  // this also flushes stderr
};

std::string currentDateAndTime();

}  // namespace util

#endif  // UTIL_LOG_H_
