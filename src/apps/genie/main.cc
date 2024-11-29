/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#define NOMINMAX
#include "genie/run/main.h"

#include <algorithm>
#include <iostream>
#include <string>

#include "cli11/CLI11.hpp"
#include "genie/capsulator/main.h"
#include "genie/gabac/main.h"
#include "genie/module/manager.h"
#include "genie/transcode-fasta/main.h"
#include "genie/transcode-fastq/main.h"
#include "genie/transcode-sam/main.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------
static void PrintCmdLine(const int argc, char* argv[]) {
  std::cerr << "genie: command line: ";
  for (int i = 0; i < argc; i++) {
    std::cerr << argv[i] << " ";
  }
  std::cerr << std::endl;
}

// -----------------------------------------------------------------------------
int stat(int, char*[]) { UTILS_DIE("Stat not implemented"); }

// -----------------------------------------------------------------------------
int help(int, char*[]) {
  std::cerr << "Usage: \ngenie <operation> <operation specific options> "
               "\n\nList of "
               "operations:\n"
            << "help\nrun\ntranscode-fastq\ntranscode-sam\n\n"
            << "To learn more about an operation, type \"genie <operation> "
               "--help\"."
            << std::endl;
  return 0;
}

// -----------------------------------------------------------------------------
int main(const int argc, char* argv[]) {
  PrintCmdLine(argc, argv);

#ifdef GENIE_USE_OPENMP
  std::cerr << "genie: built with OpenMP\n\n" << std::endl;
#else
  std::cerr << "genie: *not* built with OpenMP\n\n" << std::endl;
#endif
  genie::module::detect();
  try {
    constexpr int operation_index = 1;
    UTILS_DIE_IF(argc <= operation_index,
                 "No operation specified, type 'genie help' for more info.");
    std::string operation = argv[operation_index];
    transform(
        operation.begin(), operation.end(), operation.begin(),
        [](const char x) -> char { return static_cast<char>(tolower(x)); });
    if (operation == "run") {
      genie_app::run::main(argc - operation_index, argv + operation_index);
    } else if (operation == "stat") {
      stat(argc - operation_index, argv + operation_index);
    } else if (operation == "transcode-fasta") {
      genie_app::transcode_fasta::main(argc - operation_index,
                                       argv + operation_index);
    } else if (operation == "transcode-fastq") {
      genie_app::transcode_fastq::main(argc - operation_index,
                                       argv + operation_index);
#ifdef GENIE_SAM_SUPPORT
    } else if (operation == "transcode-sam") {
      genie_app::transcode_sam::main(argc - operation_index,
                                     argv + operation_index);
#endif
    } else if (operation == "gabac") {
      genie_app::gabac::main(argc - operation_index, argv + operation_index);
    } else if (operation == "capsulate") {
      genie_app::capsulator::main(argc - operation_index,
                                  argv + operation_index);
    } else if (operation == "help") {
      help(argc - operation_index, argv + operation_index);
    } else {
      UTILS_DIE("Unknown operation " + operation +
                "! Type \"genie help\" for a list of operations.");
    }
  } catch (const genie::util::Exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return -1;
  }
  return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
