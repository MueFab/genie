/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_STRINGVIEW_H_
#define SRC_GENIE_UTIL_STRINGVIEW_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Represents a read-only section of a string.
 */
struct StringView {
 private:
    size_t start;        //!< @brief First character's offset in the string.
    size_t stop;         //!< @brief Last character's offset plus 1.
    const char* memory;  //!< @brief String pointer.

 public:
    /**
     * @brief Create a string view without deploying to an actual string. Just save the offsets.
     * @param start_p String begin.
     * @param stop_p String end.
     */
    StringView(size_t start_p, size_t stop_p);

    /**
     * @brief Create a string view and deploys it to an actual string.
     * @param start_p String begin.
     * @param stop_p String end.
     * @param mem Pointer to string memory.
     */
    StringView(size_t start_p, size_t stop_p, const char* mem);

    /**
     * @brief Use the offsets and view at a string.
     * @param mem Pointer to the memory.
     * @return A new StringView with the string applied.
     */
    StringView deploy(const char* mem) const;

    /**
     * @return Pointer to the first character.
     */
    [[nodiscard]] const char* begin() const;

    /**
     * @return Pointer to the position after the last character.
     */
    [[nodiscard]] const char* end() const;

    /**
     * @return Length of the string view.
     */
    [[nodiscard]] size_t length() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_STRINGVIEW_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
