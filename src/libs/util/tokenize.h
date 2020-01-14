#ifndef GENIE_TOKENIZE_H
#define GENIE_TOKENIZE_H

#include <string>
#include <vector>

namespace util {

std::vector<std::string> tokenize(std::string const& str, char delim);

}

#endif  // GENIE_TOKENIZE_H
