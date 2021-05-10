#include "program-options.h"

#include <cli11/CLI11.hpp>

#include <cassert>
#include <fstream>

namespace transcoder {

ProgramOptions::ProgramOptions(int argc, char *argv[])
    : verbosity_level(0), tmp_dir_path(""), fasta_file_path(""),
      sam_file_path(""), mgrec_file_path(""){
    processCommandLine(argc, argv);
}

ProgramOptions::~ProgramOptions() = default;

void ProgramOptions::processCommandLine(int argc, char *argv[]) {
    CLI::App app{"Transcoder - Transcode legacy format to mpeg-g format"};

    app.add_option("--ref", fasta_file_path, "Reference file");
    app.add_option("--tmp", tmp_dir_path, "Temporary directory path");
    app.add_option("input", sam_file_path, "Input file")->mandatory(true);
    app.add_option("output", mgrec_file_path, "Output file")->mandatory(true);

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        if (app.count("--help")) {
            // Set our internal help flag so that the caller can act on that
            app.exit(e);
            return;
        } else {
            app.exit(e);
            throw std::runtime_error("command line parsing failed");
        }
    }

    validate();
}

void ProgramOptions::validate() const {

}

}  // namespace gabacify
