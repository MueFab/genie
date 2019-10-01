/**
 * @file
 * @copyright This file is part of Genie. See LICENSE for more details.
 */

#ifndef ENCAPSULATOR_EXCEPTIONS_H_
#define ENCAPSULATOR_EXCEPTIONS_H_

#include <exception>
#include <iostream>
#include <string>

namespace encapsulator {

class Exception : public std::exception {
   public:
    Exception() = delete;
    explicit Exception(std::string msg) : msg_(std::move(msg)) {}
    Exception(const Exception &e) noexcept : msg_(e.msg_) {}
    Exception &operator=(const Exception &) = delete;
    Exception(Exception &&) = default;
    Exception &operator=(Exception &&) = delete;
    ~Exception() noexcept override = default;

    const char *what() const noexcept override { return msg_.c_str(); }

   protected:
    std::string msg_;
};

class RuntimeError : public Exception {
   public:
    RuntimeError() = delete;
    explicit RuntimeError(const std::string &msg) : Exception(msg) {}
    RuntimeError(const RuntimeError &) = default;
    RuntimeError &operator=(const RuntimeError &) = delete;
    RuntimeError(RuntimeError &&) = default;
    RuntimeError &operator=(RuntimeError &&) = delete;
    ~RuntimeError() override = default;
};

}  // namespace encapsulator

#endif  // ENCAPSULATOR_EXCEPTIONS_H_
