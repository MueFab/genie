/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/run/program-options.h"
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

namespace genieapp::run {

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

    app.add_option("-i,--input-file", inputFile, "Input file (mgrec or mgb)\n")->mandatory(true);
    app.add_option("-o,--output-file", outputFile, "Output file (mgrec or mgb)\n")->mandatory(true);

    inputRefFile = "";
    app.add_option("-r,--input-ref-file", inputRefFile,
                   "Path to a reference fasta file. \n"
                   "Only relevant for aligned records. \nIf no path is provided, a \n"
                   "computed reference will be used instead.\n");

    workingDirectory = "";
    app.add_option("-w,--working-dir", workingDirectory,
                   "Path to a directory where \n"
                   "temporary files can be stored. \nIf no path is provided, \nthe current working dir is used. \n"
                   "Please make sure that \nenough space is available.\n");

    qvMode = "lossless";
    app.add_option("--qv", qvMode,
                   "How to encode quality values. \nPossible values are \n"
                   "\"lossless\" (default, keep all values), \n\"calq\" (quantize values with calq) and \n\"none\" "
                   "(discard all values).\n");

    readNameMode = "lossless";
    app.add_option("--read-ids", readNameMode,
                   "How to encode read ids. Possible values \n"
                   "are \"lossless\" (default, keep all values) and \n\"none\" (discard all values).\n");

    forceOverwrite = false;
    app.add_flag("-f,--force", forceOverwrite,
                 "Flag, if set already existing output \n"
                 "files are overridden.\n");

    combinePairsFlag = false;
    app.add_flag(
        "--combine-pairs", combinePairsFlag,
        "Flag, if provided to a decoding \n"
        "operation, unaligned reads will \nget matched to their mate again. \nNote: has no effect if encoded with \n"
        "--low-latency in case of aligned reads only. \nDoes not work if encoded with --read-ids \"none\"\n");

    lowLatency = false;
    app.add_flag(
        "--low-latency", lowLatency,
        "Flag, if set no global reference will be \n"
        "calculated for unaligned records. \nThis will increase encoding speed, \nbut decrease compression rate.\n");

    rawStreams = false;
    app.add_flag("--write-raw-streams", rawStreams, "Flag, if set raw uncompressed descriptors will be written out\n");

    refMode = "none";
    // Deactivated for now, as broken in connection with part 1
    /*  app.add_option("--embedded-ref", refMode,
                     "How to encode the reference. Possible \n"
                     "values are \"none\" (no encoding and reference must \nbe kept externally for decompression),\n"
                     " \"relevant\" (only parts of the reference \nneeded for decoding are encoded)\n");*/

    numberOfThreads = std::thread::hardware_concurrency();
    app.add_option("-t,--threads", numberOfThreads, "Number of threads to use.\n");

    rawReference = false;
    // Deactivated for now, as broken in connection with part 1
    /* app.add_flag("--raw-ref", rawReference,
                  "Flag, if set references will be encoded raw \n"
                  "instead of compressed. This will increase \nencoding speed, but decrease compression rate.\n");*/

    try {
        app.parse(argc, argv);

        if (workingDirectory.empty()) {
            workingDirectory = parent_dir(outputFile);
        }

        while (workingDirectory.back() == '/') {
            workingDirectory.pop_back();
        }
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

    // Initialize random number generator
    std::random_device rd;         // Seed generator
    std::mt19937 generator(rd());  // Mersenne Twister engine
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

    if (!inputSupFile.empty()) {
        validateInputFile(inputSupFile);
        inputSupFile = std::filesystem::canonical(inputSupFile).string();
        std::replace(inputSupFile.begin(), inputSupFile.end(), '\\', '/');
        std::cerr << "Input supplementary file: " << inputSupFile << " with size "
                  << size_string(std::filesystem::file_size(inputSupFile)) << std::endl;
    }
    if (!inputRefFile.empty()) {
        validateInputFile(inputRefFile);
        inputRefFile = std::filesystem::canonical(inputRefFile).string();
        std::replace(inputRefFile.begin(), inputRefFile.end(), '\\', '/');
        std::cerr << "Input reference file: " << inputRefFile << " with size "
                  << size_string(std::filesystem::file_size(inputRefFile)) << std::endl;
    }

    if (!paramsetPath.empty()) {
        validateInputFile(paramsetPath);
        std::cerr << "Parameter input file: " << paramsetPath << " with size "
                  << size_string(std::filesystem::file_size(paramsetPath)) << std::endl;
    }

    std::cerr << std::endl;

    validateWorkingDir(workingDirectory);
    workingDirectory = std::filesystem::canonical(workingDirectory).string();
    std::replace(workingDirectory.begin(), workingDirectory.end(), '\\', '/');
    std::cerr << "Working directory: " << workingDirectory << " with "
              << size_string(std::filesystem::space(workingDirectory).available) << " available" << std::endl;

    validateOutputFile(outputFile, forceOverwrite);
    if (outputFile.substr(0, 2) != "-.") {
        outputFile = std::filesystem::weakly_canonical(outputFile).string();
        std::replace(outputFile.begin(), outputFile.end(), '\\', '/');
        std::cerr << "Output file: " << outputFile << " with "
                  << size_string(std::filesystem::space(parent_dir(outputFile)).available) << " available" << std::endl;
    } else {
        std::cerr << "Output file: stdout" << std::endl;
    }

    if (!outputSupFile.empty()) {
        validateOutputFile(outputSupFile, forceOverwrite);
        outputSupFile = std::filesystem::weakly_canonical(outputSupFile).string();
        std::replace(outputSupFile.begin(), outputSupFile.end(), '\\', '/');
        std::cerr << "Output supplementary file: " << outputSupFile << " with "
                  << size_string(std::filesystem::space(parent_dir(outputSupFile)).available) << " available"
                  << std::endl;
    }

    std::cerr << std::endl;

    UTILS_DIE_IF(qvMode != "none" && qvMode != "lossless" && qvMode != "calq", "QVMode " + qvMode + " unknown");
    UTILS_DIE_IF(refMode != "none" && refMode != "relevant" && refMode != "full", "RefMode " + refMode + " unknown");
    UTILS_DIE_IF(readNameMode != "none" && readNameMode != "lossless", "Read name mode " + readNameMode + " unknown");

    if (std::thread::hardware_concurrency()) {
        UTILS_DIE_IF(numberOfThreads < 1 || numberOfThreads > std::thread::hardware_concurrency(),
                     "Invalid number of threads: " + std::to_string(numberOfThreads) +
                         ". Your system supports between 1 and " + std::to_string(std::thread::hardware_concurrency()) +
                         " threads.");
        std::cerr << "Threads: " << numberOfThreads << " with " << std::thread::hardware_concurrency() << " supported"
                  << std::endl;
    } else {
        UTILS_DIE_IF(!numberOfThreads,
                     "Could not detect hardware concurrency level. Please provide a number of threads manually.");
        std::cerr << "Threads: " << numberOfThreads << " (could not detected supported number automatically)"
                  << std::endl;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genieapp::run

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
