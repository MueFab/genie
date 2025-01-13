/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/transcode-sam/main.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "apps/genie/transcode-sam/program_options.h"
#include "genie/core/flow_graph.h"
#include "genie/format/mgrec/exporter.h"
#include "genie/format/mgrec/importer.h"
#include "genie/format/sam/exporter.h"
#include "genie/format/sam/importer.h"
#include "genie/module/default_setup.h"
#include "genie/util/stop_watch.h"
#include "util/log.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "App/TranscodeSam";

namespace genie_app::transcode_sam {

std::string file_extension(const std::string& path) {
  const auto pos = path.find_last_of('.');
  std::string ext = path.substr(pos + 1);
  for (auto& c : ext) {
    c = static_cast<char>(std::tolower(c));
  }
  return ext;
}

// -----------------------------------------------------------------------------

enum class OperationCase { UNKNOWN = 0, CONVERT = 3 };

// -----------------------------------------------------------------------------

enum class FileType { UNKNOWN = 0, MPEG = 1, THIRD_PARTY = 2 };

// -----------------------------------------------------------------------------

FileType GetType(const std::string& ext) {
  if (ext == "sam" || ext == "bam" || ext == "mgrec") {
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

template <class T>
void AttachExporter(T& flow, const ProgramOptions& p_opts,
                    std::vector<std::unique_ptr<std::ofstream>>& output_files) {
  std::ostream* file1 = &std::cout;
  if (file_extension(p_opts.output_file_) == "sam" ||
      file_extension(p_opts.output_file_) == "bam") {
    UTILS_DIE_IF(
        p_opts.output_file_ == "-.sam",
        "Sam output to stdout currently not supported.");  // TODO(renzDef)
                                                           // add after
                                                           // refactoring
                                                           // importer
    UTILS_DIE_IF(
        p_opts.output_file_ == "-.bam",
        "Bam output to stdout currently not supported.");  // TODO(renzDef)
                                                           // add after
                                                           // refactoring
                                                           // importer
    flow.AddExporter(std::make_unique<genie::format::sam::Exporter>(
        p_opts.fasta_file_path_, p_opts.output_file_));
  } else if (file_extension(p_opts.output_file_) == "mgrec") {
    if (p_opts.output_file_.substr(0, 2) != "-.") {
      output_files.emplace_back(
          std::make_unique<std::ofstream>(p_opts.output_file_));
      file1 = output_files.back().get();
    }
    flow.AddExporter(std::make_unique<genie::format::mgrec::Exporter>(*file1));
  }
}

// -----------------------------------------------------------------------------

template <class T>
void AttachImporter(T& flow, const ProgramOptions& p_opts,
                    std::vector<std::unique_ptr<std::ifstream>>& input_files,
                    std::vector<std::unique_ptr<std::ofstream>>& output_files) {
  constexpr size_t blocksize = 256000;
  if (file_extension(p_opts.input_file_) == "sam" ||
      file_extension(p_opts.input_file_) == "bam") {
    UTILS_DIE_IF(
        p_opts.input_file_ == "-.sam",
        "Sam input from stdin currently not supported.");  // TODO(renzDef)
                                                           // add after
                                                           // refactoring
                                                           // importer
    UTILS_DIE_IF(
        p_opts.input_file_ == "-.bam",
        "Bam input from stdin currently not supported.");  // TODO(renzDef)
                                                           // add after
                                                           // refactoring
                                                           // importer
    flow.AddImporter(std::make_unique<genie::format::sam::Importer>(
        blocksize, p_opts.input_file_, p_opts.fasta_file_path_));
  } else if (file_extension(p_opts.input_file_) == "mgrec") {
    auto tmp_file = p_opts.output_file_ + ".unsupported.mgrec";
    output_files.emplace_back(std::make_unique<std::ofstream>(tmp_file));

    std::istream* file1 = &std::cin;
    if (p_opts.input_file_.substr(0, 2) != "-.") {
      input_files.emplace_back(
          std::make_unique<std::ifstream>(p_opts.input_file_));
      file1 = input_files.back().get();
    }

    flow.AddImporter(std::make_unique<genie::format::mgrec::Importer>(
        blocksize, *file1, *output_files.back(), false));
  }
}

// -----------------------------------------------------------------------------

std::unique_ptr<genie::core::FlowGraph> BuildConverter(
    const ProgramOptions& p_opts,
    std::vector<std::unique_ptr<std::ifstream>>& input_files,
    std::vector<std::unique_ptr<std::ofstream>>& output_files) {
  auto flow = genie::module::build_default_converter(p_opts.num_threads_);
  AttachExporter(*flow, p_opts, output_files);
  AttachImporter(*flow, p_opts, input_files, output_files);
  return flow;
}

// -----------------------------------------------------------------------------

int main(const int argc, char* argv[]) {
  try {
    const ProgramOptions program_options(argc, argv);
    if (program_options.help_) {
      return 0;
    }

    const genie::util::Watch watch;
    std::unique_ptr<genie::core::FlowGraph> flow_graph;
    std::vector<std::unique_ptr<std::ifstream>> inputFiles;   // NOLINT
    std::vector<std::unique_ptr<std::ofstream>> outputFiles;  // NOLINT

    switch (GetOperation(program_options.input_file_,
                         program_options.output_file_)) {
      case OperationCase::UNKNOWN:
        UTILS_DIE(
            "Unknown constellation of file name extensions - don't "
            "know which operation to perform.");
      case OperationCase::CONVERT: {
        flow_graph = BuildConverter(program_options, inputFiles, outputFiles);
        break;
      }
    }

    flow_graph->Run();

    auto stats = flow_graph->GetStats();
    stats.AddDouble("time-wallclock", watch.Check());
    stats.print();
  } catch (const genie::util::Exception& e) {
    UTILS_LOG(genie::util::Logger::Severity::ERROR, e.what());
    return EXIT_FAILURE;
  } catch (const std::runtime_error& e) {
    UTILS_LOG(genie::util::Logger::Severity::ERROR, e.what());
    return EXIT_FAILURE;
  } catch (...) {
    UTILS_LOG(genie::util::Logger::Severity::ERROR, "Unknown error!");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------

}  // namespace genie_app::transcode_sam

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
