/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/conformance/program-options.h"
#include <iostream>
#include <set>
#include <string>
#include <thread>
#include <vector>
#include "cli11/CLI11.hpp"
#include "filesystem/filesystem.hpp"
#include "genie/util/runtime-exception.h"

#include <stdio.h>

namespace genieapp {
namespace conformance {

ProgramOptions::ProgramOptions(int argc, char *argv[]) : help(false) {
    CLI::App app("Genie MPEG-G capsulator\n");

    app.add_option("-i,--input-file", inputFolder, "")->mandatory(true);

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

ghc::filesystem::path ProgramOptions::getDirName(int i) const {
    auto format = "abl-%03d";
    auto size = std::snprintf(nullptr, 0, format, i);
    std::string dname(size, '\0');
    std::sprintf(&dname[0], format, i);

    return ghc::filesystem::path(inputFolder) / ghc::filesystem::path(dname);
}

// ---------------------------------------------------------------------------------------------------------------------

ghc::filesystem::path ProgramOptions::getFilePath(int i) const{
    auto format = "AbL-%03d.mgg";
    auto size = std::snprintf(nullptr, 0, format, i);
    std::string fname(size, '\0');
    std::sprintf(&fname[0], format, i);

    auto fpath = getDirName(i)/ ghc::filesystem::path(fname);

    return fpath;
}

// ---------------------------------------------------------------------------------------------------------------------

void ProgramOptions::validate() const {
//    validateInputFile(inputFile);
//    std::cout << "Input file: " << inputFile << " with size " << size_string(ghc::filesystem::file_size(inputFile))
//              << std::endl;
    UTILS_DIE_IF(!ghc::filesystem::exists(inputFolder) || !ghc::filesystem::is_directory(inputFolder),
                 "Invalid input directory!");

    for (auto i=1; i<27;i++){

        UTILS_DIE_IF(!ghc::filesystem::exists(getDirName(i)),
                     "Input dir does not exist: " + getDirName(i).string());

        UTILS_DIE_IF(!ghc::filesystem::exists(getFilePath(i)),
                     "Input file does not exist: " + getDirName(i).string());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}
}