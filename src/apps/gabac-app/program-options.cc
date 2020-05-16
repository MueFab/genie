#include "program-options.h"

#include <cli11@13becad/CLI11.hpp>

#include <genie/util/exceptions.h>
#include <cassert>
#include <fstream>

namespace gabacify {

/*
static bool fileExists(const std::string &path) {
    std::ifstream ifs(path);
    return ifs.good();
}*/

ProgramOptions::ProgramOptions(int argc, char *argv[])
    :  // RESTRUCT_DISABLE configurationFilePath(),
      logLevel(),
      inputFilePath(),
      outputFilePath(),
      task(),
      descID(0),
      subseqID(0) {
    processCommandLine(argc, argv);
}

ProgramOptions::~ProgramOptions() = default;

void ProgramOptions::processCommandLine(int argc, char *argv[]) {
    CLI::App app{"Gabacify - GABAC entropy encoder application"};

    // RESTRUCT_DISABLE app.add_option("-c,--configuration_file", this->configurationFilePath, "Configuration file");

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

void ProgramOptions::validate(void) {
    // Do stuff depending on the task
    if (this->task == "encode" || this->task == "decode") {
        // It's fine not to provide a configuration file path for encoding.
        // This will trigger the analysis.
        /* RESTRUCT_DISABLE
        UTILS_DIE_IF(this->configurationFilePath.empty() && this->inputFilePath.empty(),
                     "Configuration and input file path both not provided!");

        // We need an output file path - generate one if not provided by the
        // user
        UTILS_DIE_IF(!this->outputFilePath.empty() && fileExists(this->outputFilePath),
                     "Output file already existing: " + this->outputFilePath); */

        UTILS_DIE_IF(this->inputFilePath.empty(), "Input file path both not provided!");

        UTILS_DIE_IF(this->outputFilePath.empty(), "Output file path both not provided!");

    } /* RESTRUCT_DISABLEelse if (this->task == "analyze") {
        // We need a configuration file path - guess one if not provided by
        // the user
        UTILS_DIE_IF(!this->outputFilePath.empty() && fileExists(this->configurationFilePath),
                     "Config file already existing: " + this->outputFilePath);

        UTILS_DIE_IF(!this->configurationFilePath.empty(), "Analyze does not accept config file paths");
    } */
    else {
        UTILS_DIE("Task '" + this->task + "' is invaid");
    }
}

}  // namespace gabacify
