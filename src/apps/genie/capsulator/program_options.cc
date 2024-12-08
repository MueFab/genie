/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "apps/genie/capsulator/program_options.h"
#include <filesystem>  // NOLINT
#include <iostream>
#include <string>
#include <vector>
#include "cli11/CLI11.hpp"
#include "genie/util/runtime_exception.h"
#include "genie/util/string_helpers.h"

// -----------------------------------------------------------------------------

namespace genie_app::capsulator {

// -----------------------------------------------------------------------------
ProgramOptions::ProgramOptions(const int argc, char* argv[]) : help_(false) {
    CLI::App app("Genie MPEG-G capsulator\n");

    app.add_option("-i,--input-file", input_file_, "")->mandatory(true);
    app.add_option("-o,--output-file", output_file_, "")->mandatory(true);

    force_overwrite_ = false;
    app.add_flag("-f,--force", force_overwrite_, "");

    try {
        app.parse(argc, argv);
    } catch (const CLI::CallForHelp&) {
        std::cout << app.help() << std::endl;
        help_ = true;
        return;
    } catch (const CLI::ParseError& e) {
        UTILS_DIE("Command line parsing failed:" + std::to_string(app.exit(e)));
    }

    validate();
}

// -----------------------------------------------------------------------------
std::string parent_dir(const std::string& path) {
    std::string ret;

    if (const auto pos = path.find_last_of('/'); pos == std::string::npos) {
        ret = ".";
    } else {
        ret = path.substr(0, pos);
    }

    while (ret.back() == '/') {
        ret.pop_back();
    }
    return ret;
}

// -----------------------------------------------------------------------------
std::string size_string(const std::uintmax_t f_size) {
    size_t exponent = 0;
    auto size = static_cast<double>(f_size);
    while (size / 1024.0 > 1.0) {
        size = size / 1024.0;
        ++exponent;
    }
    const std::vector<std::string> units = {"B",   "KiB", "MiB",
                                            "GiB", "TiB", "PiB"};
    UTILS_DIE_IF(exponent >= units.size(),
                 "Filesize >= 1 exbibyte not supported");
    std::string number = std::to_string(size);
    number = number.substr(0, 4);
    if (number.back() == '.') {
        number = number.substr(0, 3);
    }
    return number + units[exponent];
}

// -----------------------------------------------------------------------------
void ValidateInputFile(const std::string& file) {
    UTILS_DIE_IF(!std::filesystem::exists(file),
                 "Input file does not exist: " + file);
    const std::ifstream stream(file);
    UTILS_DIE_IF(!stream,
                 "Input file does exist, but is not accessible. Insufficient "
                 "permissions? " +
                     file);
}

// -----------------------------------------------------------------------------
void ValidateOutputFile(const std::string& file, const bool forced) {
    UTILS_DIE_IF(std::filesystem::exists(file) && !forced,
                 "Output file already existing and no force flag set: " + file);
    UTILS_DIE_IF(
        std::filesystem::exists(file) &&
            !std::filesystem::is_regular_file(file),
        "Output file already existing, force flag set, but not a regular file. "
        "Genie won't overwrite folders "
        "or special files: " +
            file);
    {
        std::ofstream stream(file);
        const std::string test_string = "test";
        stream << test_string << std::endl;
        UTILS_DIE_IF(
            !stream,
            "Output file not accessible. Insufficient permissions? " + file);
    }
    std::filesystem::remove(file);
}

// -----------------------------------------------------------------------------
void ProgramOptions::validate() const {
    auto files = genie::util::Tokenize(input_file_, ';');

    for (const auto& f : files) {
        ValidateInputFile(f);
        std::cout << "Input file: " << f << " with Size "
                  << size_string(std::filesystem::file_size(f)) << std::endl;
    }

    //    validateOutputFile(outputFile, forceOverwrite);
    std::cout << "Output file: " << output_file_ << " with "
              << size_string(
                     std::filesystem::space(parent_dir(output_file_)).available)
              << " available" << std::endl;
}

// -----------------------------------------------------------------------------

}  // namespace genie_app::capsulator

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
