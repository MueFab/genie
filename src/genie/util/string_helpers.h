/**
 * Copyright 2018-2024 The Genie Authors.
 * @file string_helpers.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of string utility functions for manipulating and
 * processing strings.
 *
 * This file contains the declaration of various utility functions for
 * performing common string operations such as trimming, splitting, and
 * converting between binary and hexadecimal formats. These helper functions are
 * designed to simplify string manipulation and provide a consistent interface
 * for string processing tasks.
 *
 * @details The file defines functions for removing specified characters from
 * strings (`rtrim()`, `ltrim()`, and `trim()`), splitting strings into tokens
 * (`tokenize()`), and converting between binary and hexadecimal string
 * representations (`toHex()` and `fromHex()`).
 */

#ifndef SRC_GENIE_UTIL_STRING_HELPERS_H_
#define SRC_GENIE_UTIL_STRING_HELPERS_H_

// -----------------------------------------------------------------------------

#include <string>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::util {

/// Defines all known whitespace characters for trimming operations.
static const auto kWs = " \t\n\r\f\v";

/**
 * @brief Removes specified characters from the right side of a string.
 *
 * This function removes all instances of characters specified in `t` from the
 * right side of the given string `s`. By default, it removes all common
 * whitespace characters such as spaces, tabs, and newlines.
 *
 * @param s The string to be trimmed.
 * @param t A null-terminated string containing the characters to remove
 * (default is whitespace characters).
 * @return A reference to the trimmed string `s`.
 */
std::string& Rtrim(std::string& s, const char* t = kWs);

/**
 * @brief Removes specified characters from the left side of a string.
 *
 * This function removes all instances of characters specified in `t` from the
 * left side of the given string `s`. By default, it removes all common
 * whitespace characters such as spaces, tabs, and newlines.
 *
 * @param s The string to be trimmed.
 * @param t A null-terminated string containing the characters to remove
 * (default is whitespace characters).
 * @return A reference to the trimmed string `s`.
 */
std::string& Ltrim(std::string& s, const char* t = kWs);

/**
 * @brief Removes specified characters from both sides of a string.
 *
 * This function removes all instances of characters specified in `t` from both
 * the left and right sides of the given string `s`. By default, it removes all
 * common whitespace characters such as spaces, tabs, and newlines.
 *
 * @param s The string to be trimmed.
 * @param t A null-terminated string containing the characters to remove
 * (default is whitespace characters).
 * @return A reference to the trimmed string `s`.
 */
std::string& Trim(std::string& s, const char* t = kWs);

/**
 * @brief Splits a string at a delimiting character.
 *
 * This function tokenizes the given string `str` by splitting it at each
 * occurrence of the specified delimiting character `delim`. The resulting
 * tokens are stored in a vector, and the delimiting character is not included
 * in the tokens.
 *
 * @param str The input string to be split into tokens.
 * @param delim The delimiting character to split the string at (e.g., ' ' or
 * ',').
 * @return A vector containing the resulting tokens.
 */
std::vector<std::string> Tokenize(const std::string& str, char delim);

/**
 * @brief Converts a binary string to its hexadecimal representation.
 *
 * This function takes a binary string (`bin`) and converts it to a
 * corresponding hexadecimal string. Each byte in the input string is
 * represented by two hexadecimal characters in the output string.
 *
 * @param bin The binary string to be converted.
 * @return A hexadecimal string representing the binary input.
 */
std::string ToHex(const std::string& bin);

/**
 * @brief Converts a hexadecimal string to its binary representation.
 *
 * This function takes a hexadecimal string (`hex`) and converts it to a
 * corresponding binary string. Each pair of hexadecimal characters is converted
 * to a single byte in the output string.
 *
 * @param hex The hexadecimal string to be converted.
 * @return A binary string representing the hexadecimal input.
 */
std::string FromHex(const std::string& hex);

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_STRING_HELPERS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
