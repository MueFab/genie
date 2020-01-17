#include "program-options.h"

#include <genie/util/exceptions.h>
#include <cli11@13becad/CLI11.hpp>
#include <filesystem@e980ed0/filesystem.hpp>

namespace lae {

ProgramOptions::ProgramOptions(int argc, char *argv[])
    : inputFilePath(), outputFilePath(), decompression(false), num_threads(1), forceOverride(false) {
    processCommandLine(argc, argv);
}

ProgramOptions::~ProgramOptions() = default;

void ProgramOptions::processCommandLine(int argc, char *argv[]) {
    CLI::App app("Local assembly encoder");

    app.add_option("-i,--input-file", inputFilePath, "Input file")->mandatory(true);
    app.add_option("-o,--output-file", outputFilePath, "Output file")->mandatory(true);
    num_threads = 1;
    app.add_option("-t,--threads", num_threads, "Number of threads");
    app.add_flag("-f,--force-override", forceOverride, "Override output file if already existing");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        DIE("Program options error: " + std::to_string(app.exit(e)));
    }

    if (!ghc::filesystem::exists(ghc::filesystem::path(inputFilePath))) {
        DIE("Input file does not exist: " + inputFilePath);
    }

    if (!forceOverride && ghc::filesystem::exists(ghc::filesystem::path(outputFilePath))) {
        DIE("Output already existing: " + outputFilePath);
    }

    auto inDot = inputFilePath.find_last_of('.');
    if (inDot == std::string::npos) {
        DIE("Input file has no extension");
    }
    std::string inExtension = inputFilePath.substr(inDot + 1);

    auto outDot = outputFilePath.find_last_of('.');
    if (outDot == std::string::npos) {
        DIE("Output file has no extension");
    }
    std::string outExtension = outputFilePath.substr(outDot + 1);

    if (inExtension == "sam" && outExtension == "mgb") {
        std::cout << "Detected sam input + mgb output! Compressing..." << std::endl;
        decompression = false;
    } else if (inExtension == "mgb" && outExtension == "sam") {
        std::cout << "Detected mgb input + sam output! Decompressing..." << std::endl;
        decompression = true;
    } else {
        DIE("Could not infer task from input/output file extensions. Make sure to use sam input + mgb output (for "
            "compression) or mgb input + sam output (for decompression).");
    }

    std::cout << "Number of threads: " << uint32_t(num_threads) << std::endl;
}

}  // namespace lae
