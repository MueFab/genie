/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#define NOMINMAX  // NOLINT
#include "apps/genie/transcode-fasta/main.h"

#include <filesystem>  // NOLINT
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "apps/genie/transcode-fasta/program_options.h"
#include "genie/format/fasta/manager.h"
#include "genie/format/mgb/raw_reference.h"
#include "util/log.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "App/TranscodeFasta";

namespace genie_app::transcode_fasta {

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

int main(int argc, char* argv[]) {
  try {
    ProgramOptions p_opts(argc, argv);
    if (p_opts.help) {
      return 0;
    }

    auto in_ext = file_extension(p_opts.inputFile);
    auto out_ext = file_extension(p_opts.outputFile);

    if (in_ext == "mgr" && (out_ext == "fasta" || out_ext == "fa")) {
      UTILS_DIE("Mgr to fasta transcoding not yet supported");
    }
    if (!(out_ext == "mgr" && (in_ext == "fasta" || in_ext == "fa"))) {
      UTILS_DIE("Unknown combination of file extensions.");
    }

    std::string fai_name =
        p_opts.inputFile.substr(0, p_opts.inputFile.find_last_of('.')) + ".fai";
    std::string sha_name =
        p_opts.inputFile.substr(0, p_opts.inputFile.find_last_of('.')) +
        ".sha256";
    if (!std::filesystem::exists(fai_name)) {
      std::ifstream fasta_in(p_opts.inputFile);
      UTILS_DIE_IF(!fasta_in, "Cannot open file to read: " + p_opts.inputFile);
      std::ofstream fai_out(fai_name);
      genie::format::fasta::FastaReader::index(fasta_in, fai_out);
    }
    if (!std::filesystem::exists(sha_name)) {
      std::ifstream fasta_in(p_opts.inputFile);
      UTILS_DIE_IF(!fasta_in, "Cannot open file to read: " + p_opts.inputFile);
      std::ifstream fai_in(fai_name);
      UTILS_DIE_IF(!fai_in, "Cannot open file to read: " + fai_name);
      genie::format::fasta::FaiFile fai_file(fai_in);
      std::ofstream sha_out(sha_name);
      genie::format::fasta::FastaReader::hash(fai_file, fasta_in, sha_out);
    }

    auto fasta_file = std::make_unique<std::ifstream>(p_opts.inputFile);
    UTILS_DIE_IF(!fasta_file, "Cannot open file to read: " + p_opts.inputFile);
    auto fai_file = std::make_unique<std::ifstream>(fai_name);
    UTILS_DIE_IF(!fai_file, "Cannot open file to read: " + fai_name);
    auto sha_file = std::make_unique<std::ifstream>(sha_name);
    UTILS_DIE_IF(!sha_file, "Cannot open file to read: " + sha_name);
    auto ref_mgr = std::make_unique<genie::core::ReferenceManager>(4);
    [[maybe_unused]] auto fasta_mgr =
        std::make_unique<genie::format::fasta::Manager>(
            *fasta_file, *fai_file, *sha_file, ref_mgr.get(), p_opts.inputFile);
    genie::format::mgb::RawReference raw_ref(true);
    for (uint16_t i = 0;
         i < static_cast<uint16_t>(ref_mgr->GetSequences().size()); ++i) {
      auto name = ref_mgr->Id2Ref(i);
      auto length = ref_mgr->GetLength(name);
      std::string seq_str = ref_mgr->Load(name, 0, length).GetString(0, length);
      raw_ref.AddSequence(
          genie::format::mgb::RawReferenceSequence(i, 0, std::move(seq_str)));
    }

    std::ofstream outfile(p_opts.outputFile);
    genie::util::BitWriter bw(outfile);
    raw_ref.Write(bw);

    return 0;
  } catch (const std::exception& e) {
    UTILS_LOG(genie::util::Logger::Severity::ERROR, e.what());
    return 1;
  } catch (...) {
    UTILS_LOG(genie::util::Logger::Severity::ERROR, "Unknown error");
    return 1;
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie_app::transcode_fasta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
