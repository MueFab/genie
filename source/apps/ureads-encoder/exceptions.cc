#include "exceptions.h"

namespace genie {

Exception::Exception(const std::string& message) : m_message(message) {}

Exception::~Exception() noexcept = default;

std::string Exception::message() const { return m_message; }

const char* Exception::what() const noexcept { return m_message.c_str(); }

RuntimeException::RuntimeException(const std::string& file, const std::string& function, int line,
                                   const std::string& message) noexcept
    : Exception(
          // file + ":"
          // + function + ":"
          // + std::to_string(line) + ": "
          /*+ */ message) {
    // These dummy casts just avoid compiler warnings due to ununsed variables
    static_cast<void>(file);
    static_cast<void>(function);
    static_cast<void>(line);
}

RuntimeException::RuntimeException(const RuntimeException& e) noexcept : Exception(e.what()) {}

RuntimeException::~RuntimeException() noexcept = default;

}  // namespace genie
