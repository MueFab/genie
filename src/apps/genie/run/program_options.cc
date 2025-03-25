/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "apps/genie/run/program_options.h"

#include <filesystem>  // NOLINT
#include <iostream>
#include <random>
#include <string>
#include <thread>  //NOLINT
#include <vector>

#include "apps/genie/run/main.h"
#include "cli11/CLI11.hpp"
#include "genie/util/runtime_exception.h"
#include "util/log.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "App/Run";

namespace genie_app::run {

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

ProgramOptions::ProgramOptions(const int argc, char* argv[]) : help_(false) {
  CLI::App app("Genie MPEG-G reference encoder\n");

  app.add_option("-i,--input-file", input_file_,
                 "Input file (fastq or mgrec or mgb)\n")
      ->mandatory(true);
  app.add_option("-o,--output-file", output_file_,
                 "Output file (fastq or mgrec or mgb)\n")
      ->mandatory(true);

  input_sup_file_ = "";
  app.add_option("-j, --input-suppl-file", input_sup_file_,
                 "Paired input fastq file\n");

  output_sup_file_ = "";
  app.add_option("-u, --output-suppl-file", output_sup_file_,
                 "Paired output fastq file\n");

  input_ref_file_ = "";
  app.add_option(
      "-r,--input-ref-file", input_ref_file_,
      "Path to a reference fasta file. \n"
      "Only relevant for aligned records. \nIf no path is provided, a \n"
      "computed reference will be used instead.\n");

  working_directory_ = "";
  app.add_option("-w,--working-dir", working_directory_,
                 "Path to a directory where \n"
                 "temporary files can be stored. \nIf no path is provided, "
                 "\nthe current working dir is used. \n"
                 "Please make sure that \nenough space is available.\n");

  qv_mode_ = "lossless";
  app.add_option("--qv", qv_mode_,
                 "How to encode quality values. \nPossible values are \n"
                 "\"lossless\" (default, keep all values), \n\"calq\" "
                 "(quantize values with calq) and \n\"none\" "
                 "(discard all values).\n");

  read_name_mode_ = "lossless";
  app.add_option("--read-ids", read_name_mode_,
                 "How to encode read ids. Possible values \n"
                 "are \"lossless\" (default, keep all values) and \n\"none\" "
                 "(discard all values).\n");

  entropy_mode_ = "zstd";
  app.add_option("--entropy", entropy_mode_,
                 "Which entropy codec to use. Possible values \n"
                 "are \"zstd\" (default), \"gabac\", \"lzma\", \"bsc\"\n");

  force_overwrite_ = false;
  app.add_flag("-f,--force", force_overwrite_,
               "Flag, if set already existing output \n"
               "files are overridden.\n");

  combine_pairs_flag_ = false;
  app.add_flag("--combine-pairs", combine_pairs_flag_,
               "Flag, if provided to a decoding \n"
               "operation, unaligned reads will \nget matched to their mate "
               "again. \nNote: has no effect if encoded with \n"
               "--low-latency in case of aligned reads only. \nDoes not work "
               "if encoded with --read-ids \"none\"\n");

  low_latency_ = false;
  app.add_flag("--low-latency", low_latency_,
               "Flag, if set no global reference will be \n"
               "calculated for unaligned records. \nThis will increase "
               "encoding speed, \nbut decrease compression rate.\n");

  raw_streams_ = false;
  app.add_flag(
      "--write-raw-streams", raw_streams_,
      "Flag, if set raw uncompressed descriptors will be written out\n");

  ref_mode_ = "none";
  // Deactivated for now, as broken in connection with part 1
  /*  app.add_option("--embedded-ref", refMode,
                   "How to encode the reference. Possible \n"
                   "values are \"none\" (no encoding and reference must \nbe
     kept externally for decompression),\n" " \"relevant\" (only parts of the
     reference \nneeded for decoding are encoded)\n");*/

  number_of_threads_ = std::thread::hardware_concurrency();
  app.add_option("-t,--threads", number_of_threads_,
                 "Number of threads to use.\n");

  raw_reference_ = false;
  // Deactivated for now, as broken in connection with part 1
  /* app.add_flag("--raw-ref", rawReference,
                "Flag, if set references will be encoded raw \n"
                "instead of compressed. This will increase \nencoding speed, but
     decrease compression rate.\n");*/

  try {
    app.parse(argc, argv);

    if (working_directory_.empty()) {
      working_directory_ = parent_dir(output_file_);
    }

    while (working_directory_.back() == '/') {
      working_directory_.pop_back();
    }
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

void ValidatePairedFiles(const std::string& file1, const std::string& file2) {
  UTILS_DIE_IF(file_extension(file1) != file_extension(file2),
               "Input files do not have the same type.");
}

// -----------------------------------------------------------------------------

std::string random_string(const size_t length) {
  // Define the character set
  constexpr char charset[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  constexpr size_t max_index = sizeof(charset) - 1;

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
  ValidateInputFile(input_file_);
  if (input_file_.substr(0, 2) != "-.") {
    input_file_ = std::filesystem::canonical(input_file_).string();
    std::replace(input_file_.begin(), input_file_.end(), '\\', '/');
    UTILS_LOG(genie::util::Logger::Severity::INFO,
              "Input file 1: " + input_file_ + " with size " +
                  size_string(std::filesystem::file_size(input_file_)));
  } else {
    UTILS_LOG(genie::util::Logger::Severity::INFO, "Input file: stdin");
  }

  if (!input_sup_file_.empty()) {
    ValidateInputFile(input_sup_file_);
    ValidatePairedFiles(input_file_, input_sup_file_);
    input_sup_file_ = std::filesystem::canonical(input_sup_file_).string();
    std::replace(input_sup_file_.begin(), input_sup_file_.end(), '\\', '/');
    UTILS_LOG(genie::util::Logger::Severity::INFO,
              "Input file 2: " + input_sup_file_ + " with Size " +
                  size_string(std::filesystem::file_size(input_sup_file_)));
  }
  if (!input_ref_file_.empty()) {
    ValidateInputFile(input_ref_file_);
    input_ref_file_ = std::filesystem::canonical(input_ref_file_).string();
    std::replace(input_ref_file_.begin(), input_ref_file_.end(), '\\', '/');
    UTILS_LOG(genie::util::Logger::Severity::INFO,
              "Input reference file: " + input_ref_file_ + " with Size " +
                  size_string(std::filesystem::file_size(input_ref_file_)));
  }

  if (!paramset_path_.empty()) {
    ValidateInputFile(paramset_path_);
    UTILS_LOG(genie::util::Logger::Severity::INFO,
              "Parameter input file: " + paramset_path_ + " with Size " +
                  size_string(std::filesystem::file_size(paramset_path_)));
  }

  ValidateWorkingDir(working_directory_);
  working_directory_ = std::filesystem::canonical(working_directory_).string();
  std::replace(working_directory_.begin(), working_directory_.end(), '\\', '/');
  UTILS_LOG(
      genie::util::Logger::Severity::INFO,
      "Working directory: " + working_directory_ + " with " +
          size_string(std::filesystem::space(working_directory_).available) +
          " available");

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

  if (!output_sup_file_.empty()) {
    ValidateOutputFile(output_sup_file_, force_overwrite_);
    ValidatePairedFiles(output_file_, output_sup_file_);
    output_sup_file_ =
        std::filesystem::weakly_canonical(output_sup_file_).string();
    std::replace(output_sup_file_.begin(), output_sup_file_.end(), '\\', '/');
    UTILS_LOG(
        genie::util::Logger::Severity::INFO,
        "Output supplementary file: " + output_sup_file_ + " with " +
            size_string(std::filesystem::space(parent_dir(output_sup_file_))
                            .available) +
            " available");
  }

  UTILS_DIE_IF(
      qv_mode_ != "none" && qv_mode_ != "lossless" && qv_mode_ != "calq",
      "QVMode " + qv_mode_ + " unknown");
  UTILS_DIE_IF(
      ref_mode_ != "none" && ref_mode_ != "relevant" && ref_mode_ != "full",
      "RefMode " + ref_mode_ + " unknown");
  UTILS_DIE_IF(read_name_mode_ != "none" && read_name_mode_ != "lossless",
               "Read name mode " + read_name_mode_ + " unknown");
  UTILS_DIE_IF(entropy_mode_ != "gabac" && entropy_mode_ != "zstd" &&
                   entropy_mode_ != "lzma" && entropy_mode_ != "bsc",
               "Entropy mode " + entropy_mode_ + " unknown");

  if (std::thread::hardware_concurrency()) {
    UTILS_DIE_IF(
        number_of_threads_ < 1 ||
            number_of_threads_ > std::thread::hardware_concurrency(),
        "Invalid number of threads: " + std::to_string(number_of_threads_) +
            ". Your system supports between 1 and " +
            std::to_string(std::thread::hardware_concurrency()) + " threads.");
    UTILS_LOG(genie::util::Logger::Severity::INFO,
              "Threads: " + std::to_string(number_of_threads_) + " with " +
                  std::to_string(std::thread::hardware_concurrency()) +
                  " supported");
  } else {
    UTILS_DIE_IF(!number_of_threads_,
                 "Could not detect hardware concurrency level. Please provide "
                 "a number of threads manually.");
    UTILS_LOG(genie::util::Logger::Severity::INFO,
              "Threads: " + std::to_string(number_of_threads_) +
                  " (could not detected supported number automatically)");
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie_app::run

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
