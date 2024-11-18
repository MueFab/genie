/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef TEST_CONFORMANCE_HELPERS_H
#define TEST_CONFORMANCE_HELPERS_H
#include <fstream>
#include <string>

namespace util_tests {

std::string exec(const std::string& cmd);

class FileOut {
 public:
    FileOut(std::string filename, std::string extention);
    ~FileOut();
    std::ofstream& getStream() { return output; }

 private:
    std::ofstream output;
};
class FileIn {
 public:
    FileIn(std::string filename, std::string extention);
    ~FileIn();
    std::ifstream& getStream() { return input; }

 private:
    std::ifstream input;
};
}  // namespace util_tests

#endif  // TEST_CONFORMANCE_HELPERS_H
