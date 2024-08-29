/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_STRING_HELPERS_H_
#define SRC_GENIE_UTIL_STRING_HELPERS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <array>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

static const char *ws = " \t\n\r\f\v";  //!< @brief All known whitespace characters.

/**
 * @brief Removes specified characters from the right side of a string.
 * @param s String.
 * @param t List of characters to remove.
 * @return Trimmed string.
 */
std::string &rtrim(std::string &s, const char *t = ws);

/**
 * @brief Removes specified characters from the left side of a string.
 * @param s String.
 * @param t List of characters to remove.
 * @return Trimmed string.
 */
std::string &ltrim(std::string &s, const char *t = ws);

/**
 * @brief Removes specified characters from both sides of a string.
 * @param s String.
 * @param t List of characters to remove.
 * @return Trimmed string.
 */
std::string &trim(std::string &s, const char *t = ws);

/**
 * @brief Splits a string at a delimiting character.
 * @param str String to tokenize.
 * @param delim Delimiting character, e.g. ' '.
 * @return Vector of tokens. The delimiting character will not be included.
 */
std::vector<std::string> tokenize(const std::string &str, char delim);

/**
 * @brief
 * @param bin
 * @return
 */
std::string toHex(const std::string &bin);

/**
 * @brief
 * @param hex
 * @return
 */
std::string fromHex(const std::string &hex);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_STRING_HELPERS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
