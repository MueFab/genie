#ifndef UTIL_STRING_HELPERS_H_
#define UTIL_STRING_HELPERS_H_

#include <string>

namespace util {

const char *ws = " \t\n\r\f\v";

std::string &rtrim(std::string &s, const char *t = ws) {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

std::string &ltrim(std::string &s, const char *t = ws) {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

inline std::string &trim(std::string &s, const char *t = ws) { return ltrim(rtrim(s, t), t); }

}  // namespace util

#endif  // UTIL_STRING_HELPERS_H_
