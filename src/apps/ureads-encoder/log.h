#ifndef GENIE_LOG_H_
#define GENIE_LOG_H_

#include <iostream>
#include <string>

struct GenieLogTmpStdout {
    ~GenieLogTmpStdout() { std::cout << std::endl; }
};

struct GenieLogTmpStderr {
    ~GenieLogTmpStderr() { std::cout << std::endl; }
};

#define GENIE_LOG_TRACE (GenieLogTmpStdout(), std::cout << "[" << genie::currentDateAndTime() << "] [trace] ")

#define GENIE_LOG_DEBUG (GenieLogTmpStdout(), std::cout << "[" << genie::currentDateAndTime() << "] [debug] ")

#define GENIE_LOG_INFO (GenieLogTmpStdout(), std::cout << "[" << genie::currentDateAndTime() << "] [info] ")

#define GENIE_LOG_WARNING (GenieLogTmpStderr(), std::cerr << "[" << genie::currentDateAndTime() << "] [warning] ")

#define GENIE_LOG_ERROR (GenieLogTmpStderr(), std::cerr << "[" << genie::currentDateAndTime() << "] [error] ")

#define GENIE_LOG_FATAL (GenieLogTmpStderr(), std::cerr << "[" << genie::currentDateAndTime() << "] [fatal] ")

namespace genie {

std::string currentDateAndTime();

}  // namespace genie

#endif  // GENIE_LOG_H_
