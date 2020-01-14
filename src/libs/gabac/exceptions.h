/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_EXCEPTIONS_H_
#define GABAC_EXCEPTIONS_H_

/**
 * @brief Throws a gabac exception containing the current position in the code.
 */
#define GABAC_DIE(message) throw genie::gabac::RuntimeException(__FILE__, __FUNCTION__, __LINE__, message)

#include <exception>
#include <string>

namespace genie {
namespace gabac {

/**
 * @brief General exception
 */
class Exception : public std::exception {
   public:
    /**
     * @brief Create a new exception
     * @param message Reason of error
     */
    explicit Exception(const std::string& message);

    /**
     * @brief Destroy exception
     */
    ~Exception() noexcept override;

    /**
     * @brief Return the exception message
     * @return Reason of error
     */
    virtual std::string message() const;

    /**
     * @brief Return the exception message as char*
     * @return Reason of error
     */
    const char* what() const noexcept override;

   protected:
    std::string m_message;
};

/**
 * @brief Exception specialized for runtime information
 */
class RuntimeException : public Exception {
   public:
    /**
     * @brief Create new runtime exception
     * @param file File the error occurred
     * @param function Function the error occurred
     * @param line Line the error occurred
     * @param message Reason of error
     */
    explicit RuntimeException(const std::string& file, const std::string& function, int line,
                              const std::string& message) noexcept;

    /**
     * @brief Copy construction
     * @param e Source
     */
    RuntimeException(const RuntimeException& e) noexcept;

    /**
     * @brief Destroy exception
     */
    ~RuntimeException() noexcept override;
};

}  // namespace gabac
}  // namespace genie

#endif  // GABAC_EXCEPTIONS_H_
