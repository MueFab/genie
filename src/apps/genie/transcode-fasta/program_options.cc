/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "apps/genie/transcode-fasta/program_options.h"

#include <filesystem>  // NOLINT
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "cli11/CLI11.hpp"
#include "genie/util/runtime_exception.h"
#include "util/log.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "App/TranscodeFasta";

namespace genie_app::transcode_fasta {

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
ProgramOptions::ProgramOptions(const int argc, char* argv[]) : help(false) {
  CLI::App app("Genie MPEG-G reference encoder\n");

  app.add_option("-i,--input-file", inputFile, "Input file (fastq or mgrec)\n")
      ->mandatory(true);
  app.add_option("-o,--output-file", outputFile,
                 "Output file (fastq or mgrec)\n")
      ->mandatory(true);

  forceOverwrite = false;
  app.add_flag("-f,--force", forceOverwrite, "Override existing files\n");

  try {
    app.parse(argc, argv);
  } catch (const CLI::CallForHelp&) {
    UTILS_LOG(genie::util::Logger::Severity::ERROR, app.help());
    help = true;
    return;
  } catch (const CLI::ParseError& e) {
    UTILS_DIE("Command line parsing failed:" + std::to_string(app.exit(e)));
  }

  validate();
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
  if (file.substr(0, 2) == "-.") {
    return;
  }
  UTILS_DIE_IF(!std::filesystem::exists(file),
               "Input file does not exist: " + file);
  const std::ifstream stream(file);
  UTILS_DIE_IF(!stream,
               "Input file does exist, but is not accessible. Insufficient "
               "permissions? " +
                   file);
}

// -----------------------------------------------------------------------------
std::string random_string(const size_t length) {
  // Define the character set
  constexpr char charset[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  constexpr size_t max_index = sizeof(charset) - 1;

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

// -----------------------------------------------------------------------------
void ValidateWorkingDir(const std::string& dir) {
  UTILS_DIE_IF(!std::filesystem::exists(dir),
               "Directory does not exist: " + dir);
  UTILS_DIE_IF(!std::filesystem::is_directory(dir),
               "Is a file and not a directory: " + dir);

  std::string test_name;
  do {
    constexpr size_t name_length = 16;
    test_name = dir + "/" + random_string(name_length) + ".test";
  } while (std::filesystem::exists(test_name));

  {
    const std::string test_string = "test";
    std::ofstream test_file(test_name);
    test_file << test_string << std::endl;
    UTILS_DIE_IF(
        !test_file,
        "Can't write to working directory. Insufficient permissions? " + dir);
  }

  std::filesystem::remove(test_name);
}

// -----------------------------------------------------------------------------
void ValidateOutputFile(const std::string& file, const bool forced) {
  if (file.substr(0, 2) == "-.") {
    return;
  }
  UTILS_DIE_IF(std::filesystem::exists(file) && !forced,
               "Output file already existing and no force flag set: " + file);
  UTILS_DIE_IF(
      std::filesystem::exists(file) && !std::filesystem::is_regular_file(file),
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
void ProgramOptions::validate() {
  ValidateInputFile(inputFile);
  if (inputFile.substr(0, 2) != "-.") {
    inputFile = std::filesystem::canonical(inputFile).string();
    std::replace(inputFile.begin(), inputFile.end(), '\\', '/');
    UTILS_LOG(genie::util::Logger::Severity::INFO,
              "Input file: " + inputFile + " with size " +
                  size_string(std::filesystem::file_size(inputFile)));
  } else {
    UTILS_LOG(genie::util::Logger::Severity::INFO, "Input file: stdin");
  }

  ValidateOutputFile(outputFile, forceOverwrite);
  if (outputFile.substr(0, 2) != "-.") {
    outputFile = std::filesystem::weakly_canonical(outputFile).string();
    std::replace(outputFile.begin(), outputFile.end(), '\\', '/');
    UTILS_LOG(
        genie::util::Logger::Severity::INFO,
        "Output file: " + outputFile + " with " +
            size_string(
                std::filesystem::space(parent_dir(outputFile)).available) +
            " available");
  } else {
    UTILS_LOG(genie::util::Logger::Severity::INFO, "Output file: stdout");
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie_app::transcode_fasta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
