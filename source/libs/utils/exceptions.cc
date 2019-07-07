#include <utility>

#include "exceptions.h"

namespace utils {

    Exception::Exception(std::string msg) : msg_(std::move(msg)) {}

    Exception::~Exception() noexcept = default;

    std::string Exception::msg() const { return msg_; }

    const char *Exception::what() const noexcept { return msg_.c_str(); }

    RuntimeException::RuntimeException(const std::string &file, const std::string &function, int line,
                                       const std::string &msg) noexcept
            : Exception(/*file + ":" + function + ":" + std::to_string(line) + ": " + */msg) {
        // These dummy casts just avoid compiler warnings due to unused variables
        static_cast<void>(file);
        static_cast<void>(function);
        static_cast<void>(line);
    }

    RuntimeException::RuntimeException(const RuntimeException &e) noexcept : Exception(e.what()) {}

    RuntimeException::~RuntimeException() noexcept = default;

}  // namespace utils
