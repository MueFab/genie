/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_STRING_HELPERS_H_
#define SRC_GENIE_UTIL_STRING_HELPERS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include <array>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

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

inline std::string toHex(const std::string& bin) {
    static const char lut[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    std::string ret;
    for(const auto& c : bin) {
        ret += lut[(c & 0xf0) >> 4];
        ret += lut[c & 0xf];
    }
    return ret;
}

inline std::string fromHex(const std::string& hex) {
    static const std::array<char, 256> lut = []() -> std::array<char, 256> {
        std::array<char, 256> ret {};
        ret['0'] = 0;
        ret['1'] = 1;
        ret['2'] = 2;
        ret['3'] = 3;
        ret['4'] = 4;
        ret['5'] = 5;
        ret['6'] = 6;
        ret['7'] = 7;
        ret['8'] = 8;
        ret['9'] = 9;
        ret['a'] = 10;
        ret['b'] = 11;
        ret['c'] = 12;
        ret['d'] = 13;
        ret['e'] = 14;
        ret['f'] = 15;
        return ret;
    }();
    std::string ret;
    char buffer = 0;
    bool step = false;
    for(const auto& c : hex) {
        if(!step) {
            buffer = (lut[c] & 0xf) << 4;
            step = true;
        } else {
            buffer |= (lut[c] & 0xf);
            ret += buffer;
            step = false;
        }
    }
    if(step) {
        ret += buffer;
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_STRING_HELPERS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
