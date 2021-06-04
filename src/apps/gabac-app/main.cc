/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <cassert>
#include <csignal>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "apps/gabac-app/code.h"
#include "apps/gabac-app/program-options.h"
#include "genie/entropy/gabac/gabac.h"

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    try {
        gabacify::ProgramOptions programOptions(argc, argv);

        if (programOptions.task == "encode") {
            gabacify::code(programOptions.inputFilePath, programOptions.outputFilePath, programOptions.blocksize,
                           programOptions.descID, programOptions.subseqID, false, programOptions.dependencyFilePath);
        } else if (programOptions.task == "decode") {
            gabacify::code(programOptions.inputFilePath, programOptions.outputFilePath, programOptions.blocksize,
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
// ---------------------------------------------------------------------------------------------------------------------
