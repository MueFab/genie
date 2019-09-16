#ifndef UTILS_LOG_H_
#define UTILS_LOG_H_

#include <iostream>
#include <string>

#define LOG_TRACE UTILS_LOG_TRACE
#define LOG_DEBUG UTILS_LOG_DEBUG
#define LOG_INFO UTILS_LOG_INFO
#define LOG_WARNING UTILS_LOG_WARNING
#define LOG_ERROR UTILS_LOG_ERROR
#define LOG_FATAL UTILS_LOG_FATAL

#define UTILS_LOG_TRACE \
    (utils::LazyLineBreakAndFlushStdout(), std::cout << "[" << utils::currentDateAndTime() << "] [trace] ")
#define UTILS_LOG_DEBUG \
    (utils::LazyLineBreakAndFlushStdout(), std::cout << "[" << utils::currentDateAndTime() << "] [debug] ")
#define UTILS_LOG_INFO \
    (utils::LazyLineBreakAndFlushStdout(), std::cout << "[" << utils::currentDateAndTime() << "] [info] ")
#define UTILS_LOG_WARNING \
    (utils::LazyLineBreakAndFlushStdout(), std::cout << "[" << utils::currentDateAndTime() << "] [warning] ")
#define UTILS_LOG_ERROR \
    (utils::LazyLineBreakAndFlushStderr(), std::cerr << "[" << utils::currentDateAndTime() << "] [error] ")
#define UTILS_LOG_FATAL \
    (utils::LazyLineBreakAndFlushStderr(), std::cerr << "[" << utils::currentDateAndTime() << "] [fatal] ")

namespace utils {

struct LazyLineBreakAndFlushStdout {
    ~LazyLineBreakAndFlushStdout() { std::cout << std::endl; }  // this also flushes stdout
};

struct LazyLineBreakAndFlushStderr {
    ~LazyLineBreakAndFlushStderr() { std::cerr << std::endl; }  // this also flushes stderr
};

std::string currentDateAndTime();

}  // namespace utils

#endif  // UTILS_LOG_H_
