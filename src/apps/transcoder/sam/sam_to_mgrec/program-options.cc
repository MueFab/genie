/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/transcoder/sam/sam_to_mgrec/program-options.h"

#include <cassert>
#include <fstream>

#include <cli11/CLI11.hpp>

namespace genie {
namespace transcoder {
namespace sam {
namespace sam_to_mgrec {

Config::Config(int argc, char *argv[])
    : verbosity_level(0), tmp_dir_path(), fasta_file_path(), sam_file_path(), mgrec_file_path() {
    processCommandLine(argc, argv);
}

Config::~Config() = default;

void Config::processCommandLine(int argc, char *argv[]) {
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

void Config::validate() const {}

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcoder
}  // namespace genie
