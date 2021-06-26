/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "program-options.h"
#include <cassert>
#include <fstream>
#include "cli11/CLI11.hpp"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

ProgramOptions::ProgramOptions(int argc, char *argv[])
    : logLevel(), inputFilePath(), outputFilePath(), task(), blocksize(0), descID(0), subseqID(0) {
    processCommandLine(argc, argv);
}

// ---------------------------------------------------------------------------------------------------------------------

void ProgramOptions::processCommandLine(int argc, char *argv[]) {
    CLI::App app{"Gabacify - GABAC entropy encoder application"};

    this->logLevel = "info";
    app.add_option("-l,--log_level", this->logLevel,
                   "Log level: 'trace', 'info' (default), 'debug', 'warning', "
                   "'error', or 'fatal'");

    app.add_option("-i,--input_file", this->inputFilePath, "Input file");
    app.add_option("-r,--dependency_file", this->dependencyFilePath, "Reference/Dependency file");
    app.add_option("-o,--output_file", this->outputFilePath, "Output file");
    app.add_option("-t,--task", this->task, "Task ('encode' or 'decode')")->required(true);

    this->blocksize = 0;
    app.add_option("-b,--block_size", this->blocksize, "Block size - 0 means infinite");

    this->descID = 0;
    app.add_option("-d,--desc_id", this->descID, "Descriptor ID (default 0)");

    this->subseqID = 0;
    app.add_option("-s,--subseq_id", this->subseqID, "Subseq ID (default 0)");

    app.positionals_at_end(false);

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        UTILS_DIE("Program options error: " + std::to_string(app.exit(e)));
    }

    validate();
}

// ---------------------------------------------------------------------------------------------------------------------

void ProgramOptions::validate() const {
    // Do stuff depending on the task
    if (this->task == "encode" || this->task == "decode") {
        UTILS_DIE_IF(this->inputFilePath.empty(), "Input file path both not provided!");

        UTILS_DIE_IF(this->outputFilePath.empty(), "Output file path both not provided!");
    } else {
        UTILS_DIE("Task '" + this->task + "' is invalid");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
