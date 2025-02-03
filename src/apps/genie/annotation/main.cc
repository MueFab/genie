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

#include "genie/core/constants.h"
#include "genie/module/default-setup.h"
#include "genie/util/stop_watch.h"

// TODO(Fabian): For some reason, compilation on windows fails if we move this include further up. Investigate.
#include "filesystem/filesystem.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace annotation {

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    std::cerr << "Starting main annotation..\n";
    ProgramOptions programOptions(argc, argv);
    if (programOptions.task == "encode") {

        Code code(programOptions.inputFile, programOptions.outputFile, programOptions.testfileoutput,
                  programOptions.infoFields, programOptions.rec);

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
