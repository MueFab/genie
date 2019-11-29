#include "program-options.h"

#include <util/exceptions.h>
#include <cli11@13becad/CLI11.hpp>
#include <filesystem@e980ed0/filesystem.hpp>

namespace lae {

ProgramOptions::ProgramOptions(int argc, char *argv[]) : inputFilePath(), outputFilePath(), typeString("I") {
    processCommandLine(argc, argv);
}

ProgramOptions::~ProgramOptions() = default;

void ProgramOptions::processCommandLine(int argc, char *argv[]) {
    CLI::App app("Local assembly encoder");

    app.add_option("-i,--input-file", inputFilePath, "Input file")->mandatory(true);
    app.add_option("-o,--output-file", outputFilePath, "Output file");
    app.add_option("-t,--au-type", typeString, "AU Type[U, P, N, M, I]");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        DIE("Program options error: " + std::to_string(app.exit(e)));
    }

    if(typeString == "U") {
        type = format::mpegg_rec::MpeggRecord::ClassType::CLASS_U;
    } else if(typeString == "P") {
        type = format::mpegg_rec::MpeggRecord::ClassType::CLASS_P;
    } else if(typeString == "N") {
        type = format::mpegg_rec::MpeggRecord::ClassType::CLASS_N;
    } else if(typeString == "M") {
        type = format::mpegg_rec::MpeggRecord::ClassType::CLASS_M;
    } else if(typeString == "I") {
        type = format::mpegg_rec::MpeggRecord::ClassType::CLASS_I;
    } else {
        DIE("Invalid AU Type");
    }

    if (!ghc::filesystem::exists(ghc::filesystem::path(inputFilePath))) {
        DIE("Input file does not exist: " + inputFilePath);
    }

    if (ghc::filesystem::exists(ghc::filesystem::path(outputFilePath))) {
   //     DIE("Output file exists already: " + outputFilePath);
    }
}

}  // namespace lae
