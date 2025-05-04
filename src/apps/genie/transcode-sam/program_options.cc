/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/transcode-sam/program_options.h"

#include <filesystem>  // NOLINT
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <thread>  //NOLINT
#include <vector>

#include "cli11/CLI11.hpp"
#include "genie/util/runtime_exception.h"
#include "util/log.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "App/TranscodeSam";

namespace genie_app::transcode_sam {

// -----------------------------------------------------------------------------

ProgramOptions::ProgramOptions(const int argc, char* argv[])
    : verbosity_level_(0),
      force_overwrite_(false),
      help_(false),
      no_ref_(false),
      clean_(false),
      no_extended_alignment_(false),
      num_threads_(1) {
  ProcessCommandLine(argc, argv);
}

// -----------------------------------------------------------------------------

ProgramOptions::~ProgramOptions() = default;

// -----------------------------------------------------------------------------

genie::format::sam::Config ProgramOptions::ToConfig() const {
  genie::format::sam::Config config;
  config.tmp_dir_path_ = tmp_dir_path_;
  config.fasta_file_path_ = fasta_file_path_;
  config.input_file_ = input_file_;
  config.clean_ = clean_;
  config.num_threads_ = num_threads_;
  return config;
}

// -----------------------------------------------------------------------------

void ProgramOptions::ProcessCommandLine(const int argc, char* argv[]) {
  CLI::App app{"Transcoder - Transcode legacy format to mpeg-g format"};

  app.add_option("--ref", fasta_file_path_, "Path to fasta reference file\n");
  tmp_dir_path_ = "/tmp";
  app.add_option("-w,--working-dir", tmp_dir_path_,
                 "Path to a directory where temporary\n"
                 "files can be stored. If no path is provided, \n"
                 "the current working dir is used. Please make sure \n"
                 "that enough space is available.\n");
  app.add_option("-i,--input-file", input_file_, "Input file (sam or mgrec)\n")
      ->mandatory(true);
  app.add_option("-o,--output-file", output_file_,
                 "Output file (sam or mgrec)\n")
      ->mandatory(true);
  force_overwrite_ = false;
  app.add_flag("-f,--force", force_overwrite_,
               "Override existing output files\n");
  no_ref_ = false;
  app.add_flag("--no_ref", no_ref_, "Don't use a reference.\n");
  no_extended_alignment_ = false;
  app.add_flag("--no_extended_alignment", no_extended_alignment_,
               "Don't use extended alignment.\n");
  clean_ = false;
  app.add_flag("-c,--clean_records", clean_, "Remove unsupported reads.\n");
  num_threads_ = std::thread::hardware_concurrency();
  app.add_option("-t,--threads", num_threads_, "Number of threads to use.\n");
  fasta_file_path_ = "";
  app.add_option("-r,--reference", fasta_file_path_,
                 "Reference to infer the class of "
                 "MPEG records. If no reference path is supplied, classes M, N "
                 "and P are not generated. Additionally,"
                 "the reference IDs might mismatch with the actual reference "
                 "so that reference-based compression could"
                 " fail and only local assembly is available.\n");
  try {
    app.parse(argc, argv);
  } catch (const CLI::CallForHelp&) {
    UTILS_LOG(genie::util::Logger::Severity::ERROR, app.help());
    help_ = true;
    return;
  } catch (const CLI::ParseError& e) {
    UTILS_DIE("Command line parsing failed:" + std::to_string(app.exit(e)));
  }

  validate();
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

void ValidateReference(const std::string& file) {
  UTILS_DIE_IF(!std::filesystem::exists(file),
               "Reference file does not exist: " + file);
  const std::ifstream stream(file);
  UTILS_DIE_IF(!stream,
               "Input file does exist, but is not accessible. Insufficient "
               "permissions? " +
                   file);
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
               "Filesize >= 1 exbibyte not supported");  // NOLINT
  std::string number = std::to_string(size);
  number = number.substr(0, 4);
  if (number.back() == '.') {
    number = number.substr(0, 3);
  }
  return number + units[exponent];
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

std::string random_string(const size_t length) {
  // Define the character set
  constexpr char charset[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"   // NOLINT
      "abcdefghijklmnopqrstuvwxyz";  // NOLINT
  constexpr size_t max_index = sizeof(charset) - 1;

  // Use a random device to seed the random number generator
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_int_distribution<> distribution(0, max_index - 1);

  // Lambda function to generate a random character
  auto random_char = [&]() -> char { return charset[distribution(generator)]; };

  // Generate the random string
  std::string str(length, 0);
  std::generate_n(str.begin(), length, random_char);

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

void ProgramOptions::validate() {
  ValidateInputFile(input_file_);
  if (input_file_.substr(0, 2) != "-.") {
    input_file_ = std::filesystem::canonical(input_file_).string();
    std::replace(input_file_.begin(), input_file_.end(), '\\', '/');
    UTILS_LOG(genie::util::Logger::Severity::INFO,
              "Input file: " + input_file_ + " with size " +
                  size_string(std::filesystem::file_size(input_file_)));
  } else {
    UTILS_LOG(genie::util::Logger::Severity::INFO, "Input file: stdin");
  }

  ValidateOutputFile(output_file_, force_overwrite_);
  if (output_file_.substr(0, 2) != "-.") {
    output_file_ = std::filesystem::weakly_canonical(output_file_).string();
    std::replace(output_file_.begin(), output_file_.end(), '\\', '/');

    UTILS_LOG(
        genie::util::Logger::Severity::INFO,
        "Output file: " + output_file_ + " with " +
            size_string(
                std::filesystem::space(parent_dir(output_file_)).available) +
            " available");
  } else {
    UTILS_LOG(genie::util::Logger::Severity::INFO, "Output file: stdout");
  }

  ValidateWorkingDir(tmp_dir_path_);
  tmp_dir_path_ = std::filesystem::canonical(tmp_dir_path_).string();
  std::replace(tmp_dir_path_.begin(), tmp_dir_path_.end(), '\\', '/');
  UTILS_LOG(genie::util::Logger::Severity::INFO,
            "Working directory: " + tmp_dir_path_ + " with " +
                size_string(std::filesystem::space(tmp_dir_path_).available) +
                " available");

  if (std::thread::hardware_concurrency()) {
    UTILS_DIE_IF(
        num_threads_ < 1 || num_threads_ > std::thread::hardware_concurrency(),
        "Invalid number of threads: " + std::to_string(num_threads_) +
            ". Your system supports between 1 and " +
            std::to_string(std::thread::hardware_concurrency()) + " threads.");
    UTILS_LOG(genie::util::Logger::Severity::INFO,
              "Threads: " + std::to_string(num_threads_) + " with " +
                  std::to_string(std::thread::hardware_concurrency()) +
                  " supported");
  } else {
    UTILS_DIE_IF(!num_threads_,
                 "Could not detect hardware concurrency level. Please provide "
                 "a number of threads manually.");
    UTILS_LOG(genie::util::Logger::Severity::INFO,
              "Threads: " + std::to_string(num_threads_) +
                  " (could not detected supported number automatically)");
  }

  UTILS_DIE_IF(fasta_file_path_.empty() && !no_ref_ && input_file_.size() > 4 &&
                   output_file_.substr(output_file_.size() - 4) != ".sam",
               "You did not pass a reference file. "
               "Reference based compression might not work and record classes "
               "N and P can't be detected. "
               "Are you sure? If yes, pass '--no_ref'.");
  if (no_ref_) {
    fasta_file_path_ = "";
  } else {
    ValidateReference(fasta_file_path_);
    fasta_file_path_ = std::filesystem::canonical(fasta_file_path_).string();
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie_app::transcode_sam

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
