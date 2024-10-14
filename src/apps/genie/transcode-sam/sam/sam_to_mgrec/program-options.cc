/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/transcode-sam/sam/sam_to_mgrec/program-options.h"
#include <cassert>
#include <filesystem>  // NOLINT
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include "cli11/CLI11.hpp"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp::transcode_sam::sam::sam_to_mgrec {

// ---------------------------------------------------------------------------------------------------------------------

Config::Config(int argc, char *argv[])
    : verbosity_level(0),
      tmp_dir_path(),
      fasta_file_path(),
      inputFile(),
      outputFile(),
      forceOverwrite(false),
      help(false),
      no_ref(false),
      clean(false),
      num_threads(1) {
    processCommandLine(argc, argv);
}

// ---------------------------------------------------------------------------------------------------------------------

Config::~Config() = default;

// ---------------------------------------------------------------------------------------------------------------------

void Config::processCommandLine(int argc, char *argv[]) {
    CLI::App app{"Transcoder - Transcode legacy format to mpeg-g format"};

    app.add_option("--ref", fasta_file_path, "Path to fasta reference file\n");
    tmp_dir_path = "/tmp";
    app.add_option("-w,--working-dir", tmp_dir_path,
                   "Path to a directory where temporary\n"
                   "files can be stored. If no path is provided, \nthe current working dir is used. Please make sure \n"
                   "that enough space is available.\n");
    app.add_option("-i,--input-file", inputFile, "Input file (sam or mgrec)\n")->mandatory(true);
    app.add_option("-o,--output-file", outputFile, "Output file (sam or mgrec)\n")->mandatory(true);
    forceOverwrite = false;
    app.add_flag("-f,--force", forceOverwrite, "Override existing output files\n");
    no_ref = false;
    app.add_flag("--no_ref", no_ref, "Don't use a reference.\n");
    clean = false;
    app.add_flag("-c,--clean_records", clean, "Remove unsupported reads.\n");
    num_threads = std::thread::hardware_concurrency();
    app.add_option("-t,--threads", num_threads, "Number of threads to use.\n");
    fasta_file_path = "";
    app.add_option(
        "-r,--reference", fasta_file_path,
        "Reference to infer the class of "
        "MPEG records. If no reference path is supplied, classes M, N and P are not generated. Additionally,"
        "the reference IDs might mismatch with the actual reference so that reference-based compression could"
        " fail and only local assembly is available.\n");
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

void validateInputFile(const std::string &file) {
    if (file.substr(0, 2) == "-.") {
        return;
    }
    UTILS_DIE_IF(!std::filesystem::exists(file), "Input file does not exist: " + file);
    std::ifstream stream(file);
    UTILS_DIE_IF(!stream, "Input file does exist, but is not accessible. Insufficient permissions? " + file);
}

// ---------------------------------------------------------------------------------------------------------------------

void validateReference(const std::string &file) {
    UTILS_DIE_IF(!std::filesystem::exists(file), "Reference file does not exist: " + file);
    std::ifstream stream(file);
    UTILS_DIE_IF(!stream, "Input file does exist, but is not accessible. Insufficient permissions? " + file);
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

void Config::validate() {
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

    validateWorkingDir(tmp_dir_path);
    tmp_dir_path = std::filesystem::canonical(tmp_dir_path).string();
    std::replace(tmp_dir_path.begin(), tmp_dir_path.end(), '\\', '/');
    std::cerr << "Working directory: " << tmp_dir_path << " with "
              << size_string(std::filesystem::space(tmp_dir_path).available) << " available" << std::endl;

    if (std::thread::hardware_concurrency()) {
        UTILS_DIE_IF(num_threads < 1 || num_threads > std::thread::hardware_concurrency(),
                     "Invalid number of threads: " + std::to_string(num_threads) +
                         ". Your system supports between 1 and " + std::to_string(std::thread::hardware_concurrency()) +
                         " threads.");
        std::cerr << "Threads: " << num_threads << " with " << std::thread::hardware_concurrency() << " supported"
                  << std::endl;
    } else {
        UTILS_DIE_IF(!num_threads,
                     "Could not detect hardware concurrency level. Please provide a number of threads manually.");
        std::cerr << "Threads: " << num_threads << " (could not detected supported number automatically)" << std::endl;
    }

    UTILS_DIE_IF(fasta_file_path.empty() && !no_ref && inputFile.size() > 4 &&
                     outputFile.substr(outputFile.size() - 4) != ".sam",
                 "You did not pass a reference file. "
                 "Reference based compression might not work and record classes N and P can't be detected. "
                 "Are you sure? If yes, pass '--no_ref'.");
    if (no_ref) {
        fasta_file_path = "";
    } else {
        validateReference(fasta_file_path);
        fasta_file_path = std::filesystem::canonical(fasta_file_path).string();
    }

    std::cerr << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genieapp::transcode_sam::sam::sam_to_mgrec

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
