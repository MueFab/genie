#include "helpers.h"
#include "calq/string-helpers.h"

namespace calq_tests {

std::string exec(const std::string &cmd) {
    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        return "<exec(" + cmd + ") failed>";
    }

    const int bufferSize = 256;
    char buffer[bufferSize];
    std::string result;

    while (!feof(pipe)) {
        if (fgets(buffer, bufferSize, pipe) != nullptr) {
            result += buffer;
        }
    }

    pclose(pipe);

    calq::rtrim(result);

    return result;
}

}  // namespace calq_tests
