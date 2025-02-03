/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/capsulator/program-options.h"
#include <iostream>
#include <set>
#include <string>
#include <thread>
#include <vector>
#include "cli11/CLI11.hpp"
#include "filesystem/filesystem.hpp"
#include "genie/util/runtime_exception.h"
#include "genie/util/string_helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace capsulator {

// ---------------------------------------------------------------------------------------------------------------------

ProgramOptions::ProgramOptions(int argc, char *argv[]) : help(false) {
    CLI::App app("Genie MPEG-G capsulator\n");

    app.add_option("-i,--input-file", inputFile, "")->mandatory(true);
    app.add_option("-o,--output-file", outputFile, "")->mandatory(true);

    forceOverwrite = false;
    app.add_flag("-f,--force", forceOverwrite, "");

    try {
        app.parse(argc, argv);
    } catch (const CLI::CallForHelp &) {
        std::cout << app.help() << std::endl;
        help = true;
        return;
    } catch (const CLI::ParseError &e) {
        UTILS_DIE("Command line parsing failed:" + std::to_string(app.exit(e)));
    }

    validate();
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

void ProgramOptions::validate() const {
    auto files = genie::util::tokenize(inputFile, ';');

    for (const auto &f : files) {
        validateInputFile(f);
        std::cout << "Input file: " << f << " with size " << size_string(ghc::filesystem::file_size(f)) << std::endl;
    }

    //    validateOutputFile(outputFile, forceOverwrite);
    std::cout << "Output file: " << outputFile << " with "
              << size_string(ghc::filesystem::space(parent_dir(outputFile)).available) << " available" << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace capsulator
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
