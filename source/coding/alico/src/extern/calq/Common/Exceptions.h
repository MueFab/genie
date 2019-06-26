/** @file Exceptions.h
 *  @brief This files contains the definitions of some custom exception
 *         classes.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#ifndef CALQ_COMMON_EXCEPTIONS_H_
#define CALQ_COMMON_EXCEPTIONS_H_

#include <exception>
#include <iostream>
#include <string>

#include "Common/helpers.h"

namespace calq {

class Exception : public std::exception {
 public:
    explicit Exception(const std::string &msg);
    virtual ~Exception(void) throw();
    virtual std::string getMessage(void) const;
    virtual const char * what(void) const throw();

 protected:
    std::string msg_;
};

class ErrorException : public Exception {
 public:
    explicit ErrorException(const std::string &msg): Exception(msg) {}
};

inline void throwErrorException(const std::string &msg) {
    std::cout.flush();
    throw ErrorException(msg);
}

class ErrorExceptionReporter {
 public:
    ErrorExceptionReporter(const std::string &file,
                           const std::string &function,
                           const int &line)
        : file_(file)
        , function_(function)
        , line_(line)
    {}

    void operator()(const std::string &msg) {
//         std::cerr << file << ":" << function << ":" << line << ": ";
        std::string tmp = fileBaseName(file_) + ":" + function_ + ":" + std::to_string(line_) + ": " + msg;
        // Can use the original name here, as it is still defined
        throwErrorException(tmp);
    }

 private:
    std::string file_;
    std::string function_;
    int line_;
};

}  // namespace calq

// Remove the symbol for the function, then define a new version that instead
// creates a stack temporary instance of ErrorExceptionReporter initialized
// with the caller.
#undef throwErrorException
#define throwErrorException calq::ErrorExceptionReporter(__FILE__, __FUNCTION__, __LINE__)

#endif  // CALQ_COMMON_EXCEPTIONS_H_

