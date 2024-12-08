/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "apps/genie/gabac/program_options.h"

#include <cassert>
#include <fstream>

#include "cli11/CLI11.hpp"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie_app::gabac {

// -----------------------------------------------------------------------------
ProgramOptions::ProgramOptions(const int argc, char* argv[])
    : fast_benchmark_(false), blocksize_(0), desc_id_(0), subseq_id_(0) {
  ProcessCommandLine(argc, argv);
}

// -----------------------------------------------------------------------------
void ProgramOptions::ProcessCommandLine(const int argc, char* argv[]) {
  CLI::App app{"Gabacify - GABAC entropy encoder application"};

  this->log_level_ = "info";
  app.add_option("-l,--log_level", this->log_level_,
                 "Log level: 'trace', 'info' (default), 'debug', 'warning', "
                 "'error', or 'fatal'");

  app.add_option("-i,--input_file", this->input_file_path_, "Input file");
  app.add_option("-c,--config_file", this->param_file_path_,
                 "Task ('encode' or 'Decode')");
  app.add_option("-r,--dependency_file", this->dependency_file_path_,
                 "Reference/Dependency file");
  app.add_option("-o,--output_file", this->output_file_path_, "Output file");
  app.add_option("-t,--task", this->task_, "Task ('encode' or 'Decode')");
  app.add_flag("--fast-benchmark", this->fast_benchmark_, "Optimize for speed");

  this->blocksize_ = 0;
  // app.add_option("-b,--block_size", this->blocksize, "Block Size - 0 means
  // infinite");

  this->desc_id_ = 0;
  app.add_option("-d,--desc_id", this->desc_id_, "Descriptor ID (default 0)");

  this->subseq_id_ = 0;
  app.add_option("-s,--subseq_id", this->subseq_id_, "Subseq ID (default 0)");

  app.positionals_at_end(false);

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    UTILS_DIE("Program options error: " + std::to_string(app.exit(e)));
  }

  validate();
}

// -----------------------------------------------------------------------------
void ProgramOptions::validate() const {
  // Do stuff depending on the task
  if (this->task_ == "encode" || this->task_ == "Decode") {
    UTILS_DIE_IF(this->input_file_path_.empty(),
                 "Input file path both not provided!");

    UTILS_DIE_IF(this->output_file_path_.empty(),
                 "Output file path both not provided!");
  } else if (this->task_ == "writeconfigs" || this->task_ == "benchmark") {
  } else {
    UTILS_DIE("Task '" + this->task_ + "' is invalid");
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie_app::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
