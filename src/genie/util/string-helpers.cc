/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "string-helpers.h"

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

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------