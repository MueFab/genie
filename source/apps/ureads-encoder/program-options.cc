#include "program-options.h"

#include "exceptions.h"
#include "log.h"
#include "logger.h"

#include <cli11@13becad/CLI11.hpp>
#include <filesystem@e980ed0/filesystem.hpp>

namespace genie {

ProgramOptions::ProgramOptions(int argc, char* argv[]) : inputFilePath(), outputFilePath() {
    processCommandLine(argc, argv);
}

ProgramOptions::~ProgramOptions() = default;

void ProgramOptions::processCommandLine(int argc, char* argv[]) {
    Logger& logger = Logger::instance();
    logger.out("Processing command line options");

    CLI::App app("Genie MPEG-G encoder, conformance version");

    app.add_option("-i,--input_file_path", inputFilePath, "Input file path")->mandatory(true);
    app.add_option("-o,--output_file_path", outputFilePath, "Output file path")->mandatory(true);

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        GENIE_DIE("Program options error: " + std::to_string(app.exit(e)));
    }

    if (!ghc::filesystem::exists(ghc::filesystem::path(inputFilePath))) {
        GENIE_DIE("Input file does not exist");
    }

    if (ghc::filesystem::exists(ghc::filesystem::path(outputFilePath))) {
        GENIE_DIE("Output file exists already");
    }
}

}  // namespace genie
