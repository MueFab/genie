#include "tokenize.h"

std::vector<std::string> tokenize(std::string const& str, const char delim) {
    size_t start;
    size_t end = 0;
    std::vector<std::string> out;
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
    return out;
}