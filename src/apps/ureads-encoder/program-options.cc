#include "program-options.h"

#include "util/exceptions.h"

#include <cli11@13becad/CLI11.hpp>
#include <filesystem@e980ed0/filesystem.hpp>

namespace ureads_encoder {

ProgramOptions::ProgramOptions(int argc, char* argv[]) : inputFilePath(), outputFilePath() {
    processCommandLine(argc, argv);
}

ProgramOptions::~ProgramOptions() = default;

void ProgramOptions::processCommandLine(int argc, char* argv[]) {
    std::cout << "Processing command line options" << std::endl;

    CLI::App app("Genie MPEG-G encoder, conformance version");

    app.add_option("-i,--input-file", inputFilePath, "Input file")->mandatory(true);
    app.add_option("-p,--pair-file", pairFilePath, "pair file");
    app.add_option("-o,--output-file", outputFilePath, "Output file")->mandatory(true);
    app.add_option("-q,--pair-output-file", outputPairFilePath, "pair file");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        UTILS_DIE("Program options error: " + std::to_string(app.exit(e)));
    }

    if (!ghc::filesystem::exists(ghc::filesystem::path(inputFilePath))) {
        UTILS_DIE("Input file does not exist");
    }

    if (!ghc::filesystem::exists(ghc::filesystem::path(pairFilePath))) {
        UTILS_DIE("pair file does not exist");
    }
}

}  // namespace ureads_encoder
