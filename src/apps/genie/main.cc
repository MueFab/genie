/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#define NOMINMAX
#include "genie/run/main.h"
#include <iostream>
#include <string>
#include "cli11/CLI11.hpp"
#include "genie/gabac/main.h"
#include "genie/annotation/main.h"
#include "genie/util/runtime-exception.h"
#include "genie/module/manager.h"
//#include "genie/capsulator/main.h"
//#include "genie/transcode-fasta/main.h"
//#include "genie/transcode-fastq/main.h"
//#include "genie/transcode-sam/main.h"

// ---------------------------------------------------------------------------------------------------------------------

static void printCmdLine(int argc, char* argv[]) {
    std::cerr << "genie: command line: ";
    for (int i = 0; i < argc; i++) {
        std::cerr << argv[i] << " ";
    }
    std::cerr << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

int stat(int, char*[]) { UTILS_DIE("Stat not implemented"); }

// ---------------------------------------------------------------------------------------------------------------------
const int versionnr = 1;
int help(int, char*[]) {
    std::cerr << "Version: " << std::to_string(versionnr) << "\n\n"
              << "Usage: \ngenie <operation> <operation specific options> \n\nList of operations:\n"
              << "help\nrun\nannotation\ntranscode-fastq\ntranscode-sam\n\n"
              << "To learn more about an operation, type \"genie <operation> --help\"." << std::endl;
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    printCmdLine(argc, argv);

#ifdef GENIE_USE_OPENMP
    std::cerr << "genie: built with OpenMP\n\n" << std::endl;
#else
    std::cerr << "genie: *not* built with OpenMP\n\n" << std::endl;
#endif
    genie::module::detect();
    constexpr int OPERATION_INDEX = 1;
    try {
        UTILS_DIE_IF(argc <= OPERATION_INDEX, "No operation specified, type 'genie help' for more info.");
        std::string operation = argv[OPERATION_INDEX];
        transform(operation.begin(), operation.end(), operation.begin(),
                  [](char x) -> char { return static_cast<char>(tolower(x)); });
        if (operation == "run") {
            genieapp::run::main(argc - OPERATION_INDEX, argv + OPERATION_INDEX);
        } else if (operation == "stat") {
            stat(argc - OPERATION_INDEX, argv + OPERATION_INDEX);
        } else if (operation == "gabac") {
            genieapp::gabac::main(argc - OPERATION_INDEX, argv + OPERATION_INDEX);
        } else if (operation == "annotation") {
            genieapp::annotation::main(argc - OPERATION_INDEX, argv + OPERATION_INDEX);
            UTILS_DIE("not yet implemented");
        } else if (operation == "help") {
            help(argc - OPERATION_INDEX, argv + OPERATION_INDEX);
        } else {
            UTILS_DIE("Unknown operation " + operation + "! Type \"genie help\" for a list of operations.");
        }
    } catch (const genie::util::Exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
