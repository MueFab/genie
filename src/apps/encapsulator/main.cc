/**
 * @file
 * @copyright This file is part of Genie. See LICENSE for more details.
 */

#include <iostream>
#include <sstream>
#include <vector>
#include "FileHandlingUtils.h"
#include "exceptions.h"
#include "program-options.h"

static std::string commandLineStr(int argc, char* argv[]) {
    std::vector<std::string> args(argv, (argv + argc));
    std::stringstream commandLine;
    for (const auto& arg : args) {
        commandLine << arg << " ";
    }
    return commandLine.str();
}

static int encapsulator_main(int argc, char* argv[]) {
    try {
        encapsulator::ProgramOptions programOptions(argc, argv);
        if (programOptions.help) {
            return 0;
        }
        std::cout << "command line: " + commandLineStr(argc, argv) << std::endl;

        createMPEGGFileNoMITFromByteStream(programOptions.inputFilePath.c_str(), nullptr);

    } catch (const encapsulator::RuntimeError& runtimeError) {
        std::cerr << "error: encapsulator: " << runtimeError.what() << std::endl;
        return -1;
    } catch (const std::exception& stdException) {
        std::cerr << "error: " << stdException.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "error: unknown error occurred" << std::endl;
        return -1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    int rc = encapsulator_main(argc, argv);
    if (rc != 0) {
        std::cerr << "error: failed to run encapsulator" << std::endl;
    }

    return ((rc == 0) ? EXIT_SUCCESS : EXIT_FAILURE);
}
