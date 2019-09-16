#include "program-options.h"

#include <gabac/gabac.h>

#include <cli11/cli11.h>

#include <cassert>
#include <fstream>

namespace gabacify {

static bool fileExists(const std::string &path) {
    std::ifstream ifs(path);
    return ifs.good();
}

ProgramOptions::ProgramOptions(int argc, char *argv[])
    : configurationFilePath(),
      logLevel(),
      inputFilePath(),
      outputFilePath(),
      task() {
    processCommandLine(argc, argv);
}

ProgramOptions::~ProgramOptions() = default;

void ProgramOptions::processCommandLine(int argc, char *argv[]) {
    CLI::App app{"Gabacify - GABAC entropy encoder application"};

    app.add_option("-c,--configuration_file", this->configurationFilePath,
                   "Configuration file");

    this->logLevel = "info";
    app.add_option("-l,--log_level", this->logLevel,
                   "Log level: 'trace', 'info' (default), 'debug', 'warning', "
                   "'error', or 'fatal'");

    app.add_option("-i,--input_file", this->inputFilePath,
                   "Input file");
    app.add_option("-o,--output_file", this->outputFilePath,
                   "Output file");
    app.add_option("task,-t,--task", this->task,
                   "Task ('encode' or 'decode' or 'analyze')")
        ->required(true);

    this->blocksize = 0;
    app.add_option("-b,--block_size", this->blocksize,
                   "Block size - 0 means infinite");

    this->maxVal = 0;
    app.add_option("-v,--value_max", this->maxVal,
                   "Maximum value - 0 means automatic");

    this->wordSize = 0;
    app.add_option("-w,--word_size", this->wordSize,
                   "Word size - 0 means automatic");

    app.positionals_at_end(false);

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        GABAC_DIE("Program options error: " + std::to_string(app.exit(e)));
    }

    validate();
}

void ProgramOptions::validate(void) {
    // Do stuff depending on the task
    if (this->task == "encode" || this->task == "decode") {
        // It's fine not to provide a configuration file path for encoding.
        // This will trigger the analysis.
        if (this->configurationFilePath.empty() &&
            this->inputFilePath.empty()) {
            GABAC_DIE("Configuration and input file path both not provided!");
        }

        // We need an output file path - generate one if not provided by the
        // user
        if (!this->outputFilePath.empty()) {
            if (fileExists(this->outputFilePath)) {
                GABAC_DIE("Output file already existing: " +
                          this->outputFilePath);
            }
        }

    } else if (this->task == "analyze") {
        // We need a configuration file path - guess one if not provided by
        // the user
        if (!this->outputFilePath.empty()) {
            if (fileExists(this->configurationFilePath)) {
                GABAC_DIE("Config file already existing: " +
                          this->outputFilePath);
            }
        }

        if (!this->configurationFilePath.empty()) {
            GABAC_DIE("Analyze does not accept config file paths");
        }

    } else {
        GABAC_DIE("Task '" + this->task + "' is invaid");
    }
}

}  // namespace gabacify
