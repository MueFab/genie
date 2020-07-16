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
 *
 */
class Exception : public std::exception {
   public:
    /**
     *
     * @param msg
     */
    explicit Exception(std::string msg);

    /**
     *
     */
    ~Exception() noexcept override;

    /**
     *
     * @return
     */
    virtual std::string msg() const;

    /**
     *
     * @return
     */
    const char *what() const noexcept override;

   protected:
    std::string msg_;  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // UTIL_EXCEPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------