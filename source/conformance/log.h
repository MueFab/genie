#ifndef GENIE_LOG_H_
#define GENIE_LOG_H_


#include <iostream>
#include <string>


struct GENIELogTmpStdout {
    ~GENIELogTmpStdout() { std::cout << std::endl; }
};


struct GENIELogTmpStderr {
    ~GENIELogTmpStderr() { std::cout << std::endl; }
};


#define GENIE_LOG_TRACE (GENIELogTmpStdout(), std::cout << "[" << genie::currentDateAndTime() << "] [trace] ")

#define GENIE_LOG_DEBUG (GENIELogTmpStdout(), std::cout << "[" << genie::currentDateAndTime() << "] [debug] ")

#define GENIE_LOG_INFO (GENIELogTmpStdout(), std::cout << "[" << genie::currentDateAndTime() << "] [info] ")

#define GENIE_LOG_WARNING (GENIELogTmpStderr(), std::cerr << "[" << genie::currentDateAndTime() << "] [warning] ")

#define GENIE_LOG_ERROR (GENIELogTmpStderr(), std::cerr << "[" << genie::currentDateAndTime() << "] [error] ")

#define GENIE_LOG_FATAL (GENIELogTmpStderr(), std::cerr << "[" << genie::currentDateAndTime() << "] [fatal] ")


namespace genie {


    std::string currentDateAndTime();


}  // namespace genie


#endif  // GENIE_LOG_H_
