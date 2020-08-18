/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef UTIL_EXCEPTIONS_H_
#define UTIL_EXCEPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 * @brief A general exception.
 */
class Exception : public std::exception {
   public:
    /**
     * @brief Initialize with a message.
     * @param msg The message.
     */
    explicit Exception(std::string msg) noexcept;

    /**
     * @brief Nothrow copy constructor
     * @param e Source object
     */
    Exception(const Exception& e) noexcept;

    /**
     * @brief Needed for inheritance.
     */
    ~Exception() noexcept override = default;

    /**
     * @return The internal message.
     */
    virtual std::string msg() const;

    /**
     * @return The internal message as a c string.
     */
    const char* what() const noexcept override;

   protected:
    std::string msg_;  //!< @brief The exception message.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // UTIL_EXCEPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------