/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/util/string_helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

std::string &rtrim(std::string &s, const char *t) {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string &ltrim(std::string &s, const char *t) {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string &trim(std::string &s, const char *t) { return ltrim(rtrim(s, t), t); }

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::string> tokenize(const std::string &str, const char delim) {
    size_t start;
    size_t end = 0;
    std::vector<std::string> out;
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
    return out;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string toHex(const std::string &bin) {
    static const char lut[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    std::string ret;
    for (const auto &c : bin) {
        ret += lut[(c & 0xf0) >> 4];
        ret += lut[c & 0xf];
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string fromHex(const std::string &hex) {
    static const std::array<char, 256> lut = []() -> std::array<char, 256> {
        std::array<char, 256> ret{};
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
    for (const auto &c : hex) {
        if (!step) {
            buffer = (lut[c] & 0xf) << 4;
            step = true;
        } else {
            buffer |= (lut[c] & 0xf);
            ret += buffer;
            step = false;
        }
    }
    if (step) {
        ret += buffer;
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
