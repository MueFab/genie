/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include "helpers.h"
#include "genie/util/string_helpers.h"

namespace util_tests {

std::string exec(const std::string &cmd) {
#ifdef WIN32
    FILE *pipe = _popen(cmd.c_str(), "r");
#else
    FILE *pipe = popen(cmd.c_str(), "r");
#endif
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
#ifdef WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    genie::util::rtrim(result);

    return result;
}

FileOut::FileOut(std::string filename, std::string extension) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    if (extension == "json" || extension == "txt")
        output.open(gitRootDir + filename + "." + extension);
    else
        output.open(gitRootDir + filename + "." + extension, std::ios::binary);
}

FileOut::~FileOut() {
    if (output.is_open()) output.close();
}

FileIn::FileIn(std::string filename, std::string extension) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    if (extension == "json" || extension == "txt")
        input.open(gitRootDir + filename + "." + extension);
    else
        input.open(gitRootDir + filename + "." + extension, std::ios::binary);
}

FileIn::~FileIn() {
    if (input.is_open()) input.close();
}

}  // namespace util_tests