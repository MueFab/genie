/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_STRINGVIEW_H
#define GENIE_STRINGVIEW_H

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 *
 */
struct StringView {
   private:
    size_t start; //!<
    size_t stop; //!<
    const char* memory; //!<

   public:
    /**
     *
     * @param start_p
     * @param stop_p
     */
    StringView(size_t start_p, size_t stop_p);

    /**
     *
     * @param start_p
     * @param stop_p
     * @param mem
     */
    StringView(size_t start_p, size_t stop_p, const char* mem);

    /**
     *
     * @param mem
     * @return
     */
    StringView deploy(const char* mem) const;

    /**
     *
     * @return
     */
    const char* begin() const;

    /**
     *
     * @return
     */
    const char* end() const;

    /**
     *
     * @return
     */
    size_t length() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_STRINGVIEW_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------