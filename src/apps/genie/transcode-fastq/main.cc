/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#define NOMINMAX
#include "apps/genie/transcode-fastq/main.h"

#include <filesystem>  // NOLINT
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "apps/genie/transcode-fastq/program_options.h"
#include "genie/format/fasta/exporter.h"
#include "genie/format/fastq/exporter.h"
#include "genie/format/fastq/importer.h"
#include "genie/format/mgrec/exporter.h"
#include "genie/format/mgrec/importer.h"
#include "genie/module/default_setup.h"
#include "genie/util/stop_watch.h"
#include "util/log.h"

constexpr auto kLogModuleName = "App/TranscodeFastq";

// -----------------------------------------------------------------------------

namespace genie_app::transcode_fastq {


// -----------------------------------------------------------------------------
enum class OperationCase { UNKNOWN = 0, CONVERT = 3 };

// -----------------------------------------------------------------------------
enum class FileType { UNKNOWN = 0, MPEG = 1, THIRD_PARTY = 2 };

// -----------------------------------------------------------------------------
FileType GetType(const std::string& ext) {
  if (ext == "fastq" || ext == "mgrec") {
    return FileType::THIRD_PARTY;
  }
  if (ext == "mgb") {
    return FileType::MPEG;
  }
  return FileType::UNKNOWN;
}

// -----------------------------------------------------------------------------
OperationCase GetOperation(const FileType in, const FileType out) {
  if (in == FileType::THIRD_PARTY && out == FileType::THIRD_PARTY) {
    return OperationCase::CONVERT;
  }
  return OperationCase::UNKNOWN;
}

// -----------------------------------------------------------------------------
OperationCase GetOperation(const std::string& filename_in,
                           const std::string& filename_out) {
  return GetOperation(GetType(file_extension(filename_in)),
                      GetType(file_extension(filename_out)));
}

// -----------------------------------------------------------------------------
template <class T>
void AttachExporter(T& flow, const ProgramOptions& p_opts,
                    std::vector<std::unique_ptr<std::ofstream>>& output_files) {
  std::ostream* file1 = &std::cout;
  if (p_opts.outputFile.substr(0, 2) != "-.") {
    output_files.emplace_back(
        std::make_unique<std::ofstream>(p_opts.outputFile));
    file1 = output_files.back().get();
  }
  if (file_extension(p_opts.outputFile) == "fastq") {
    if (file_extension(p_opts.outputSupFile) == "fastq") {
      std::ostream* file2 = &std::cout;
      if (p_opts.outputSupFile.substr(0, 2) != "-.") {
        output_files.emplace_back(
            std::make_unique<std::ofstream>(p_opts.outputSupFile));
        file2 = output_files.back().get();
      }
      flow.AddExporter(
          std::make_unique<genie::format::fastq::Exporter>(*file1, *file2));
    } else {
      flow.AddExporter(
          std::make_unique<genie::format::fastq::Exporter>(*file1));
    }
  } else if (file_extension(p_opts.outputFile) == "mgrec") {
    flow.AddExporter(std::make_unique<genie::format::mgrec::Exporter>(*file1));
  } else if (file_extension(p_opts.outputFile) == "fasta") {
    flow.AddExporter(std::make_unique<genie::format::fasta::Exporter>(
        &flow.GetRefMgr(), file1, p_opts.numberOfThreads));
  }
}

// -----------------------------------------------------------------------------
template <class T>
void AttachImporter(T& flow, const ProgramOptions& p_opts,
                    std::vector<std::unique_ptr<std::ifstream>>& input_files,
                    std::vector<std::unique_ptr<std::ofstream>>& output_files) {
  constexpr size_t blocksize = 256000;
  std::istream* file1 = &std::cin;
  if (p_opts.inputFile.substr(0, 2) != "-.") {
    input_files.emplace_back(std::make_unique<std::ifstream>(p_opts.inputFile));
    UTILS_DIE_IF(!input_files.back(),
                     "Cannot open file to read: " + p_opts.inputFile);
    file1 = input_files.back().get();
  }
  if (file_extension(p_opts.inputFile) == "fastq") {
    if (file_extension(p_opts.inputSupFile) == "fastq") {
      std::istream* file2 = &std::cin;
      if (p_opts.inputSupFile.substr(0, 2) != "-.") {
        input_files.emplace_back(
            std::make_unique<std::ifstream>(p_opts.inputSupFile));
        UTILS_DIE_IF(!input_files.back(),
                     "Cannot open file to read: " + p_opts.inputSupFile);
        file2 = input_files.back().get();
      }
      flow.AddImporter(std::make_unique<genie::format::fastq::Importer>(
          blocksize, *file1, *file2));
    } else {
      flow.AddImporter(
          std::make_unique<genie::format::fastq::Importer>(blocksize, *file1));
    }
  } else if (file_extension(p_opts.inputFile) == "mgrec") {
    auto tmp_file = p_opts.outputFile + ".unsupported.mgrec";
    output_files.emplace_back(std::make_unique<std::ofstream>(tmp_file));
    flow.AddImporter(std::make_unique<genie::format::mgrec::Importer>(
        blocksize, *file1, *output_files.back(), false));
    std::remove(tmp_file.c_str());
  }
}

// -----------------------------------------------------------------------------
std::unique_ptr<genie::core::FlowGraph> BuildConverter(
    const ProgramOptions& p_opts,
    std::vector<std::unique_ptr<std::ifstream>>& input_files,
    std::vector<std::unique_ptr<std::ofstream>>& output_files) {
  auto flow = genie::module::build_default_converter(p_opts.numberOfThreads);
  AttachExporter(*flow, p_opts, output_files);
  AttachImporter(*flow, p_opts, input_files, output_files);
  return flow;
}

// -----------------------------------------------------------------------------
int main(const int argc, char* argv[]) {
  try {
    const ProgramOptions p_opts(argc, argv);
    if (p_opts.help) {
      return 0;
    }
    const genie::util::Watch watch;
    std::unique_ptr<genie::core::FlowGraph> flow_graph;
    std::vector<std::unique_ptr<std::ifstream>> inputFiles;   // NOLINT
    std::vector<std::unique_ptr<std::ofstream>> outputFiles;  // NOLINT
    switch (GetOperation(p_opts.inputFile, p_opts.outputFile)) {
      case OperationCase::UNKNOWN:
        UTILS_DIE(
            "Unknown constellation of file name extensions - don't "
            "know which operation to perform.");
      case OperationCase::CONVERT: {
        flow_graph = BuildConverter(p_opts, inputFiles, outputFiles);
        break;
      }
    }

    flow_graph->Run();

    auto stats = flow_graph->GetStats();
    stats.AddDouble("time-wallclock", watch.Check());
    stats.print();

    return 0;
  } catch (std::exception& e) {
    UTILS_LOG(genie::util::Logger::Severity::ERROR, e.what());
    return 1;
  } catch (...) {
    UTILS_LOG(genie::util::Logger::Severity::ERROR, "Error - Unknown");
    return 1;
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie_app::transcode_fastq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
