#include "program-options.h"

#include <util/exceptions.h>
#include <cli11@13becad/CLI11.hpp>
#include <filesystem@e980ed0/filesystem.hpp>

namespace lae {

ProgramOptions::ProgramOptions(int argc, char *argv[]) : inputFilePath(), outputFilePath() {
    processCommandLine(argc, argv);
}

ProgramOptions::~ProgramOptions() = default;

void ProgramOptions::processCommandLine(int argc, char *argv[]) {
    CLI::App app("Local assembly encoder");

    app.add_option("-i,--input-file", inputFilePath, "Input file")->mandatory(true);
    app.add_option("-o,--output-file", outputFilePath, "Output file");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        DIE("Program options error: " + std::to_string(app.exit(e)));
    }

    if (!ghc::filesystem::exists(ghc::filesystem::path(inputFilePath))) {
        DIE("Input file does not exist: " + inputFilePath);
    }

    if (ghc::filesystem::exists(ghc::filesystem::path(outputFilePath))) {
   //     DIE("Output file exists already: " + outputFilePath);
    }
}

}  // namespace lae
