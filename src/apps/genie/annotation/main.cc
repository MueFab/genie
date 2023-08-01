/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#define NOMINMAX
#include "apps/genie/annotation/main.h"
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "apps/genie/annotation/code.h"
#include "apps/genie/annotation/program-options.h"

#include "genie/module/default-setup.h"
#include "genie/util/watch.h"
#include "genie/core/constants.h"

// TODO(Fabian): For some reason, compilation on windows fails if we move this include further up. Investigate.
#include "filesystem/filesystem.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace annotation {

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    ProgramOptions programOptions(argc, argv);
    if (programOptions.task == "encode") {
        std::cerr << "calling Code...\n";
        genie::core::AlgoID encodeMode = genie::core::AlgoID::BSC;
        Code code(programOptions.inputFile,
                  programOptions.outputFile,
                  encodeMode,
                  programOptions.testfileoutput);
    } else if (programOptions.task == "decode") {
        UTILS_DIE("not yet implemented: " + std::string(programOptions.task));
    } else {
        UTILS_DIE("Invalid task: " + std::string(programOptions.task));
    }
    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
