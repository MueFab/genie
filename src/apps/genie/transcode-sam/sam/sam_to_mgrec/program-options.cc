/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "program-options.h"

// ---------------------------------------------------------------------------------------------------------------------

#include <cassert>
#include <fstream>

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/runtime-exception.h"
#include <cli11/CLI11.hpp>
#include <filesystem/filesystem.hpp>

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace transcode_sam {
namespace sam {
namespace sam_to_mgrec {

// ---------------------------------------------------------------------------------------------------------------------

Config::Config(int argc, char *argv[])
    : verbosity_level(0), tmp_dir_path(), fasta_file_path(), inputFile(), outputFile() {
    processCommandLine(argc, argv);
}

// ---------------------------------------------------------------------------------------------------------------------

Config::~Config() = default;

// ---------------------------------------------------------------------------------------------------------------------

void Config::processCommandLine(int argc, char *argv[]) {
    CLI::App app{"Transcoder - Transcode legacy format to mpeg-g format"};

    app.add_option("--ref", fasta_file_path, "Reference file");
    app.add_option("-w,--working-dir", tmp_dir_path, "Temporary directory path");
    app.add_option("-i,--input-file", inputFile, "Input file")->mandatory(true);
    app.add_option("-o,--output-file", outputFile, "Output file")->mandatory(true);
    forceOverwrite = false;
    app.add_flag("-f,--force", forceOverwrite, "");
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

// ---------------------------------------------------------------------------------------------------------------------

void validateInputFile(const std::string &file) {
    UTILS_DIE_IF(!ghc::filesystem::exists(file), "Input file does not exist: " + file);
    std::ifstream stream(file);
    UTILS_DIE_IF(!stream, "Input file does exist, but is not accessible. Insufficient permissions? " + file);
}

// ---------------------------------------------------------------------------------------------------------------------

void validateOutputFile(const std::string &file, bool forced) {
    UTILS_DIE_IF(ghc::filesystem::exists(file) && !forced,
                 "Output file already existing and no force flag set: " + file);
    UTILS_DIE_IF(ghc::filesystem::exists(file) && !ghc::filesystem::is_regular_file(file),
                 "Output file already existing, force flag set, but not a regular file. Genie won't overwrite folders "
                 "or special files: " +
                 file);
    {
        std::ofstream stream(file);
        const std::string TEST_STRING = "test";
        stream << TEST_STRING << std::endl;
        UTILS_DIE_IF(!stream, "Output file not accessible. Insufficient permissions? " + file);
    }
    ghc::filesystem::remove(file);
}

// ---------------------------------------------------------------------------------------------------------------------

std::string size_string(std::uintmax_t f_size) {
    size_t exponent = 0;
    auto size = static_cast<double>(f_size);
    while (size / 1024.0 > 1.0) {
        size = size / 1024.0;
        ++exponent;
    }
    const std::vector<std::string> UNITS = {"B", "KiB", "MiB", "GiB", "TiB", "PiB"};
    UTILS_DIE_IF(exponent >= UNITS.size(), "Filesize >= 1 exbibyte not supported");
    std::string number = std::to_string(size);
    number = number.substr(0, 4);
    if (number.back() == '.') {
        number = number.substr(0, 3);
    }
    return number + UNITS[exponent];
}

// ---------------------------------------------------------------------------------------------------------------------

std::string parent_dir(const std::string &path) {
    std::string ret;

    auto pos = path.find_last_of('/');
    if (pos == std::string::npos) {
        ret = ".";
    } else {
        ret = path.substr(0, pos);
    }

    while (ret.back() == '/') {
        ret.pop_back();
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void Config::validate() {
    validateInputFile(inputFile);
    std::cout << "Input file: " << inputFile << " with size " << size_string(ghc::filesystem::file_size(inputFile))
              << std::endl;

    std::cout << std::endl;

    validateOutputFile(outputFile, forceOverwrite);
    std::cout << "Output file: " << outputFile << " with "
              << size_string(ghc::filesystem::space(parent_dir(outputFile)).available) << " available" << std::endl;


    std::cout << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcode_sam
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
