/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/gabac/main.h"
#include <cassert>
#include <csignal>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "apps/genie/gabac/code.h"
#include "apps/genie/gabac/program-options.h"
#include "genie/entropy/gabac/gabac.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    try {
        ProgramOptions programOptions(argc, argv);

        if (programOptions.task == "encode") {
            code(programOptions.inputFilePath, programOptions.outputFilePath, programOptions.blocksize,
                 programOptions.descID, programOptions.subseqID, false, programOptions.dependencyFilePath);
        } else if (programOptions.task == "decode") {
            code(programOptions.inputFilePath, programOptions.outputFilePath, programOptions.blocksize,
                 programOptions.descID, programOptions.subseqID, true, programOptions.dependencyFilePath);
        } else {
            UTILS_DIE("Invalid task: " + std::string(programOptions.task));
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unkown error occurred" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
