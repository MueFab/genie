/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#define NOMINMAX
#include "apps/genie/gabac/main.h"

#include <cassert>
#include <csignal>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "apps/genie/gabac/code.h"
#include "apps/genie/gabac/program_options.h"
#include "genie/entropy/gabac/benchmark.h"
#include "genie/entropy/gabac/gabac.h"
#include "util/log.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "App/Gabac";
namespace genie_app::gabac {

// -----------------------------------------------------------------------------

int main(int argc, char* argv[]) {
  try {
    if (ProgramOptions program_options(argc, argv);
        program_options.task_ == "encode") {
      code(program_options.input_file_path_, program_options.output_file_path_,
           program_options.param_file_path_, program_options.blocksize_,
           program_options.desc_id_, program_options.subseq_id_, false,
           program_options.dependency_file_path_);
    } else if (program_options.task_ == "Decode") {
      code(program_options.input_file_path_, program_options.output_file_path_,
           program_options.param_file_path_, program_options.blocksize_,
           program_options.desc_id_, program_options.subseq_id_, true,
           program_options.dependency_file_path_);
    } else if (program_options.task_ == "writeconfigs") {
      for (const auto& d : genie::core::GetDescriptors()) {
        for (const auto& s : d.sub_seqs) {
          auto conf = genie::entropy::gabac::EncodingConfiguration(
                          genie::entropy::gabac::GetEncoderConfigManual(s.id))
                          .ToJson()
                          .dump(4);
          std::ofstream outstream(
              "gabacconf_" + std::to_string(static_cast<uint8_t>(s.id.first)) +
              "_" + std::to_string(s.id.second) + ".json");
          outstream << conf;
        }
      }
    } else if (program_options.task_ == "benchmark") {
      float timeweight = 0.0f;
      if (program_options.fast_benchmark_) {
        timeweight = 1.0f;
      }
      auto result = genie::entropy::gabac::BenchmarkFull(
          program_options.input_file_path_,
          genie::core::GenSubIndex(std::make_pair(
              static_cast<genie::core::GenDesc>(program_options.desc_id_),
              program_options.subseq_id_)),
          timeweight);
      auto json = result.config.ToJson().dump(4);
      std::ofstream output_stream(program_options.output_file_path_);
      output_stream.write(json.c_str(),
                          static_cast<std::streamsize>(json.length()));

    } else {
      UTILS_DIE("Invalid task: " + std::string(program_options.task_));
    }
  } catch (const std::exception& e) {
    UTILS_LOG(genie::util::Logger::Severity::ERROR, e.what());
    return EXIT_FAILURE;
  } catch (...) {
    UTILS_LOG(genie::util::Logger::Severity::ERROR, "Unknown error occurred");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------

}  // namespace genie_app::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
