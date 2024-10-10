/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "apps/genie/transcode-fasta/program-options.h"
#include <filesystem>  // NOLINT
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <thread>
#include <vector>
#include "cli11/CLI11.hpp"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp::transcode_fasta {

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

ProgramOptions::ProgramOptions(int argc, char *argv[]) : help(false) {
    CLI::App app("Genie MPEG-G reference encoder\n");

    app.add_option("-i,--input-file", inputFile, "Input file (fastq or mgrec)\n")->mandatory(true);
    app.add_option("-o,--output-file", outputFile, "Output file (fastq or mgrec)\n")->mandatory(true);

    forceOverwrite = false;
    app.add_flag("-f,--force", forceOverwrite, "Override existing files\n");

    try {
        app.parse(argc, argv);
    } catch (const CLI::CallForHelp &) {
        std::cerr << app.help() << std::endl;
        help = true;
        return;
    } catch (const CLI::ParseError &e) {
        UTILS_DIE("Command line parsing failed:" + std::to_string(app.exit(e)));
    }

    validate();
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
    if (file.substr(0, 2) == "-.") {
        return;
    }
    UTILS_DIE_IF(!std::filesystem::exists(file), "Input file does not exist: " + file);
    std::ifstream stream(file);
    UTILS_DIE_IF(!stream, "Input file does exist, but is not accessible. Insufficient permissions? " + file);
}

// ---------------------------------------------------------------------------------------------------------------------

std::string random_string(size_t length) {
    // Define the character set
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = sizeof(charset) - 1;

    // Use a random device to seed the random number generator
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, max_index - 1);

    // Lambda function to generate a random character
    auto randchar = [&]() -> char { return charset[distribution(generator)]; };

    // Generate the random string
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);

    return str;
}

// ---------------------------------------------------------------------------------------------------------------------

void validateWorkingDir(const std::string &dir) {
    UTILS_DIE_IF(!std::filesystem::exists(dir), "Directory does not exist: " + dir);
    UTILS_DIE_IF(!std::filesystem::is_directory(dir), "Is a file and not a directory: " + dir);

    std::string test_name;
    do {
        const size_t NAME_LENGTH = 16;
        test_name = dir + "/" + random_string(NAME_LENGTH) + ".test";
    } while (std::filesystem::exists(test_name));

    {
        const std::string TEST_STRING = "test";
        std::ofstream test_file(test_name);
        test_file << TEST_STRING << std::endl;
        UTILS_DIE_IF(!test_file, "Can't write to working directory. Insufficient permissions? " + dir);
    }

    std::filesystem::remove(test_name);
}

// ---------------------------------------------------------------------------------------------------------------------

void validateOutputFile(const std::string &file, bool forced) {
    if (file.substr(0, 2) == "-.") {
        return;
    }
    UTILS_DIE_IF(std::filesystem::exists(file) && !forced,
                 "Output file already existing and no force flag set: " + file);
    UTILS_DIE_IF(std::filesystem::exists(file) && !std::filesystem::is_regular_file(file),
                 "Output file already existing, force flag set, but not a regular file. Genie won't overwrite folders "
                 "or special files: " +
                     file);
    {
        std::ofstream stream(file);
        const std::string TEST_STRING = "test";
        stream << TEST_STRING << std::endl;
        UTILS_DIE_IF(!stream, "Output file not accessible. Insufficient permissions? " + file);
    }
    std::filesystem::remove(file);
}

// ---------------------------------------------------------------------------------------------------------------------

void ProgramOptions::validate() {
    validateInputFile(inputFile);
    if (inputFile.substr(0, 2) != "-.") {
        inputFile = std::filesystem::canonical(inputFile).string();
        std::replace(inputFile.begin(), inputFile.end(), '\\', '/');
        std::cerr << "Input file: " << inputFile << " with size " << size_string(std::filesystem::file_size(inputFile))
                  << std::endl;
    } else {
        std::cerr << "Input file: stdin" << std::endl;
    }

    std::cerr << std::endl;

    validateOutputFile(outputFile, forceOverwrite);
    if (outputFile.substr(0, 2) != "-.") {
        outputFile = std::filesystem::weakly_canonical(outputFile).string();
        std::replace(outputFile.begin(), outputFile.end(), '\\', '/');
        std::cerr << "Output file: " << outputFile << " with "
                  << size_string(std::filesystem::space(parent_dir(outputFile)).available) << " available" << std::endl;
    } else {
        std::cerr << "Output file: stdout" << std::endl;
    }

    std::cerr << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genieapp::transcode_fasta

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
