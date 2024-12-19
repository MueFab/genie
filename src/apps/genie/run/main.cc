/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#define NOMINMAX  // NOLINT
#include "apps/genie/run/main.h"

#include <filesystem>  // NOLINT
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "apps/genie/run/program_options.h"
#include "genie/core/format_importer_null.h"
#include "genie/core/name_encoder_none.h"
#include "genie/format/fasta/exporter.h"
#include "genie/format/fasta/manager.h"
#include "genie/format/fastq/exporter.h"
#include "genie/format/fastq/importer.h"
#include "genie/format/mgb/exporter.h"
#include "genie/format/mgb/importer.h"
#include "genie/format/mgrec/exporter.h"
#include "genie/format/mgrec/importer.h"
#include "genie/format/sam/exporter.h"
#include "genie/format/sam/importer.h"
#include "genie/module/default_setup.h"
#include "genie/quality/calq/encoder.h"
#include "genie/quality/qvwriteout/encoder_none.h"
#include "genie/read/lowlatency/encoder.h"
#include "genie/util/stop_watch.h"
#include "util/log.h"

constexpr auto kLogModuleName = "App/Run";

// -----------------------------------------------------------------------------

namespace genie_app::run {

// -----------------------------------------------------------------------------
std::string file_extension(const std::string& path) {
  const auto pos = path.find_last_of('.');
  std::string ext = path.substr(pos + 1);
  for (auto& c : ext) {
    c = static_cast<char>(std::tolower(c));
  }
  return ext;
}

// -----------------------------------------------------------------------------
enum class OperationCase { UNKNOWN = 0, ENCODE = 1, DECODE = 2, CAPSULATE = 4 };

// -----------------------------------------------------------------------------
enum class FileType { UNKNOWN = 0, MPEG = 1, THIRD_PARTY = 2 };

// -----------------------------------------------------------------------------
FileType GetType(const std::string& ext) {
  if (ext == "mgrec" || ext == "fasta" || ext == "fastq" || ext == "sam") {
    return FileType::THIRD_PARTY;
  }
  if (ext == "mgb") {
    return FileType::MPEG;
  }
  return FileType::UNKNOWN;
}

// -----------------------------------------------------------------------------
OperationCase GetOperation(const FileType in, const FileType out) {
  if (in == FileType::THIRD_PARTY && out == FileType::MPEG) {
    return OperationCase::ENCODE;
  }
  if (in == FileType::MPEG && out == FileType::THIRD_PARTY) {
    return OperationCase::DECODE;
  }
  if (in == FileType::MPEG && out == FileType::MPEG) {
    return OperationCase::CAPSULATE;
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
  } else if (file_extension(p_opts.outputFile) == "sam") {
    flow.AddExporter(std::make_unique<genie::format::sam::Exporter>(
        p_opts.inputRefFile, p_opts.outputFile));
  } else if (file_extension(p_opts.outputFile) == "fasta") {
    flow.AddExporter(std::make_unique<genie::format::fasta::Exporter>(
        &flow.GetRefMgr(), file1, p_opts.numberOfThreads));
  }
}

// -----------------------------------------------------------------------------
void AddFasta(const std::string& fasta_file_path,
              genie::core::FlowGraphEncode* flow,
              std::vector<std::unique_ptr<std::ifstream>>& input_files) {
  std::string fai =
      fasta_file_path.substr(0, fasta_file_path.find_last_of('.') + 1) + "fai";
  std::string sha =
      fasta_file_path.substr(0, fasta_file_path.find_last_of('.') + 1) +
      "sha256";
  auto fasta_file = std::make_unique<std::ifstream>(fasta_file_path);
  UTILS_DIE_IF(!fasta_file, "Cannot open file to read: " + fasta_file_path);
  if (!std::filesystem::exists(fai)) {
    UTILS_LOG(genie::util::Logger::Severity::INFO,
              "Indexing " + fasta_file_path);
    std::ofstream fai_file(fai);
    genie::format::fasta::FastaReader::index(*fasta_file, fai_file);
  }
  if (!std::filesystem::exists(sha)) {
    UTILS_LOG(genie::util::Logger::Severity::INFO,
              "Calculating hashes " + fasta_file_path);
    std::ofstream sha_file(sha);
    std::ifstream fai_file(fai);
    UTILS_DIE_IF(!fai_file, "Cannot open file to read: " + fai);
    genie::format::fasta::FaiFile fai_reader(fai_file);
    genie::format::fasta::FastaReader::hash(fai_reader, *fasta_file, sha_file);
  }
  auto fai_file = std::make_unique<std::ifstream>(fai);
  UTILS_DIE_IF(!fai_file, "Cannot open file to read: " + fai);
  auto sha_file = std::make_unique<std::ifstream>(sha);
  UTILS_DIE_IF(!sha_file, "Cannot open file to read: " + sha);
  input_files.push_back(std::move(fasta_file));
  input_files.push_back(std::move(fai_file));
  input_files.push_back(std::move(sha_file));
  flow->AddReferenceSource(std::make_unique<genie::format::fasta::Manager>(
      **(input_files.rbegin() + 2), **(input_files.rbegin() + 1),
      **input_files.rbegin(), &flow->GetRefMgr(), fasta_file_path));
}

// -----------------------------------------------------------------------------
template <class T>
void AttachImporterMgrec(
    T& flow, const ProgramOptions& p_opts,
    std::vector<std::unique_ptr<std::ifstream>>& input_files,
    std::vector<std::unique_ptr<std::ofstream>>& output_files) {
  std::istream* in_ptr = &std::cin;
  if (p_opts.inputFile.substr(0, 2) != "-.") {
    input_files.emplace_back(std::make_unique<std::ifstream>(p_opts.inputFile));
    UTILS_DIE_IF(!input_files.back(),
                 "Cannot open file to read: " + p_opts.inputFile);
    in_ptr = input_files.back().get();
  }

  if (file_extension(p_opts.inputFile) == "mgrec") {
    constexpr size_t block_size = 128000;
    output_files.emplace_back(std::make_unique<std::ofstream>(
        p_opts.outputFile + ".unsupported.mgrec"));
    flow.AddImporter(std::make_unique<genie::format::mgrec::Importer>(
        block_size, *in_ptr, *output_files.back()));
  } else if (file_extension(p_opts.inputFile) == "fasta") {
    flow.AddImporter(std::make_unique<genie::core::NullImporter>());
  }
}

// -----------------------------------------------------------------------------
template <class T>
void AttachImporterFastq(
    T& flow, const ProgramOptions& p_opts,
    std::vector<std::unique_ptr<std::ifstream>>& input_files) {
  constexpr size_t block_size = 256000;
  std::istream* file1 = &std::cin;
  if (p_opts.inputFile.substr(0, 2) != "-.") {
    input_files.emplace_back(std::make_unique<std::ifstream>(p_opts.inputFile));
    UTILS_DIE_IF(!input_files.back(),
                 "Cannot open file to read: " + p_opts.inputFile);
    file1 = input_files.back().get();
  }
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
        block_size, *file1, *file2));
  } else {
    flow.AddImporter(
        std::make_unique<genie::format::fastq::Importer>(block_size, *file1));
  }
}

template <class T>
void AttachImporterSam(
    T& flow, const ProgramOptions& p_opts,
    std::vector<std::unique_ptr<std::ifstream>>& input_files) {
  constexpr size_t block_size = 128000;
  // std::istream* in_ptr = &std::cin;
  if (p_opts.inputFile.substr(0, 2) != "-.") {
    input_files.emplace_back(std::make_unique<std::ifstream>(p_opts.inputFile));
    // in_ptr = inputFiles.back().get();
  }
  flow.AddImporter(std::make_unique<genie::format::sam::Importer>(
      block_size, p_opts.inputFile, p_opts.inputRefFile));
}

// -----------------------------------------------------------------------------
std::unique_ptr<genie::core::FlowGraph> BuildEncoder(
    const ProgramOptions& p_opts,
    std::vector<std::unique_ptr<std::ifstream>>& input_files,
    std::vector<std::unique_ptr<std::ofstream>>& output_files) {
  constexpr size_t block_size = 128000;
  genie::core::ClassifierRegroup::RefMode mode;
  if (p_opts.refMode == "none") {
    mode = genie::core::ClassifierRegroup::RefMode::kNone;
  } else if (p_opts.refMode == "full") {
    mode = genie::core::ClassifierRegroup::RefMode::kFull;
  } else {
    mode = genie::core::ClassifierRegroup::RefMode::kRelevant;
  }
  if (file_extension(p_opts.inputFile) == "fasta") {
    mode = genie::core::ClassifierRegroup::RefMode::kFull;
  }
  auto flow = genie::module::BuildDefaultEncoder(
      p_opts.numberOfThreads, p_opts.workingDirectory, block_size, mode,
      p_opts.rawReference, p_opts.rawStreams);
  if (file_extension(p_opts.inputFile) == "fasta") {
    AddFasta(p_opts.inputFile, flow.get(), input_files);
  } else if (!p_opts.inputRefFile.empty()) {
    if (file_extension(p_opts.inputRefFile) == "fasta" ||
        file_extension(p_opts.inputRefFile) == "fa") {
      AddFasta(p_opts.inputRefFile, flow.get(), input_files);
    } else {
      UTILS_DIE("Unknown reference format");
    }
  }
  std::ostream* out_ptr = &std::cout;
  if (p_opts.outputFile.substr(0, 2) != "-.") {
    output_files.emplace_back(
        std::make_unique<std::ofstream>(p_opts.outputFile, std::ios::binary));
    out_ptr = output_files.back().get();
  }
  flow->AddExporter(std::make_unique<genie::format::mgb::Exporter>(out_ptr));
  if (file_extension(p_opts.inputFile) == "fastq") {
    AttachImporterFastq(*flow, p_opts, input_files);
  } else if (file_extension(p_opts.inputFile) == "sam") {
    AttachImporterSam(*flow, p_opts, input_files);
  } else {
    AttachImporterMgrec(*flow, p_opts, input_files, output_files);
  }
  if (p_opts.qvMode == "none") {
    flow->SetQvCoder(
        std::make_unique<genie::quality::qvwriteout::NoneEncoder>(), 0);
  }
  if (p_opts.qvMode == "calq") {
    flow->SetQvCoder(std::make_unique<genie::quality::calq::Encoder>(), 0);
  }
  if (p_opts.readNameMode == "none") {
    flow->SetNameCoder(std::make_unique<genie::core::NameEncoderNone>(), 0);
  }
  if (p_opts.lowLatency) {
    flow->SetReadCoder(
        std::make_unique<genie::read::lowlatency::Encoder>(p_opts.rawStreams),
        3);
    flow->SetReadCoder(
        std::make_unique<genie::read::lowlatency::Encoder>(p_opts.rawStreams),
        4);
  }
  return flow;
}

// -----------------------------------------------------------------------------
std::unique_ptr<genie::core::FlowGraph> BuildDecoder(
    const ProgramOptions& p_opts,
    std::vector<std::unique_ptr<std::ifstream>>& input_files,
    std::vector<std::unique_ptr<std::ofstream>>& output_files) {
  auto flow = genie::module::build_default_decoder(
      p_opts.numberOfThreads, p_opts.workingDirectory, p_opts.combinePairsFlag);

  std::string json_uri_path = p_opts.inputRefFile;
  if (std::filesystem::exists(p_opts.inputFile + ".json") &&
      std::filesystem::file_size(p_opts.inputFile + ".json")) {
    if (genie::core::meta::Dataset data(
            nlohmann::json::parse(std::ifstream(p_opts.inputFile + ".json")));
        data.GetReference()) {
      std::string uri =
          dynamic_cast<const genie::core::meta::external_ref::Fasta&>(
              data.GetReference()->GetBase())
              .GetUri();
      std::string scheme = "file://";
      UTILS_DIE_IF(uri.substr(0, scheme.length()) != scheme,
                   "Unknown URI scheme: " + uri);
      std::string path = uri.substr(scheme.length());
      UTILS_LOG(genie::util::Logger::Severity::INFO,
                "Extracted reference URI " + uri);
      if (std::filesystem::exists(path)) {
        UTILS_LOG(genie::util::Logger::Severity::INFO,
                  "Reference URI valid" + uri);
        json_uri_path = path;
      } else {
        UTILS_LOG(
            genie::util::Logger::Severity::WARNING,
            "Reference URI invalid. Falling back to CLI reference." + uri);
      }
    }
  }
  if (!json_uri_path.empty()) {
    if (file_extension(json_uri_path) == "fasta" ||
        file_extension(json_uri_path) == "fa") {
      std::string fai =
          json_uri_path.substr(0, json_uri_path.find_last_of('.') + 1) + "fai";
      std::string sha =
          json_uri_path.substr(0, json_uri_path.find_last_of('.') + 1) +
          "sha256";
      auto fasta_file = std::make_unique<std::ifstream>(json_uri_path);
      if (!std::filesystem::exists(fai)) {
        UTILS_LOG(genie::util::Logger::Severity::INFO,
                  "Indexing " + json_uri_path);
        std::ofstream fai_file(fai);
        genie::format::fasta::FastaReader::index(*fasta_file, fai_file);
      }
      if (!std::filesystem::exists(sha)) {
        UTILS_LOG(genie::util::Logger::Severity::INFO,
                  "Calculating hashes " + json_uri_path);
        std::ofstream sha_file(sha);
        std::ifstream fai_file(fai);
        UTILS_DIE_IF(!fai_file, "Cannot open file to read: " + fai);
        genie::format::fasta::FaiFile fai_reader(fai_file);
        genie::format::fasta::FastaReader::hash(fai_reader, *fasta_file,
                                                sha_file);
      }
      auto fai_file = std::make_unique<std::ifstream>(fai);
      auto sha_file = std::make_unique<std::ifstream>(sha);
      input_files.push_back(std::move(fasta_file));
      input_files.push_back(std::move(fai_file));
      input_files.push_back(std::move(sha_file));
      flow->AddReferenceSource(std::make_unique<genie::format::fasta::Manager>(
          **(input_files.rbegin() + 2), **(input_files.rbegin() + 1),
          **input_files.rbegin(), &flow->GetRefMgr(), json_uri_path));
    } else if (file_extension(json_uri_path) == "mgb") {
      input_files.emplace_back(std::make_unique<std::ifstream>(json_uri_path));
      flow->AddImporter(std::make_unique<genie::format::mgb::Importer>(
          *input_files.back(), &flow->GetRefMgr(), flow->GetRefDecoder(),
          true));
    }
  }
  std::istream* in_ptr = &std::cin;
  if (p_opts.inputFile.substr(0, 2) != "-.") {
    input_files.emplace_back(
        std::make_unique<std::ifstream>(p_opts.inputFile, std::ios::binary));
    in_ptr = input_files.back().get();
  }
  flow->AddImporter(std::make_unique<genie::format::mgb::Importer>(
      *in_ptr, &flow->GetRefMgr(), flow->GetRefDecoder(),
      file_extension(p_opts.outputFile) == "fasta"));
  AttachExporter(*flow, p_opts, output_files);
  return flow;
}

// -----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
  try {
    ProgramOptions p_opts(argc, argv);
    if (p_opts.help) {
      return 0;
    }
    genie::util::Watch watch;
    std::unique_ptr<genie::core::FlowGraph> flow_graph;
    std::vector<std::unique_ptr<std::ifstream>> input_files;
    std::vector<std::unique_ptr<std::ofstream>> output_files;
    switch (GetOperation(p_opts.inputFile, p_opts.outputFile)) {
      case OperationCase::UNKNOWN:
        UTILS_DIE(
            "Unknown constellation of file name extensions - don't "
            "know which operation to perform.");
      case OperationCase::ENCODE:
        flow_graph = BuildEncoder(p_opts, input_files, output_files);
        break;
      case OperationCase::DECODE:
        flow_graph = BuildDecoder(p_opts, input_files, output_files);
        break;
      case OperationCase::CAPSULATE:
        UTILS_DIE("Encapsulation / Decapsulation not yet supported.");
    }

    flow_graph->Run();

    if (GetOperation(p_opts.inputFile, p_opts.outputFile) ==
        OperationCase::ENCODE) {
      std::ofstream jsonfile(p_opts.outputFile + ".json");
      auto json_string = flow_graph->GetMeta().ToJson().dump(4);
      jsonfile.write(json_string.data(),
                     static_cast<std::streamsize>(json_string.length()));
    }

    auto stats = flow_graph->GetStats();
    stats.AddDouble("time-wallclock", watch.Check());
    stats.print();

    return 0;
  } catch (std::exception& e) {
    UTILS_LOG(genie::util::Logger::Severity::ERROR, e.what());
    return 1;
  } catch (...) {
    UTILS_LOG(genie::util::Logger::Severity::ERROR, "Unknown error");
    return 1;
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie_app::run

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
