/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_APPS_GENIE_TRANSCODE_FASTQ_PROGRAM_OPTIONS_H_
#define SRC_APPS_GENIE_TRANSCODE_FASTQ_PROGRAM_OPTIONS_H_

// -----------------------------------------------------------------------------

#include <string>

// -----------------------------------------------------------------------------

namespace genie_app::transcode_fastq {

/**
 * @brief
 */
class ProgramOptions {
 public:
  /**
   * @brief
   * @param argc
   * @param argv
   */
  ProgramOptions(int argc, char* argv[]);

 public:
  std::string inputFile;     //!< @brief
  std::string inputSupFile;  //!< @brief

  std::string outputFile;     //!< @brief
  std::string outputSupFile;  //!< @brief

  bool forceOverwrite;  //!< @brief

  size_t numberOfThreads;  //!< @brief

  bool help;  //!< @brief

 private:
  /**
   * @brief
   */
  void validate();
};

// -----------------------------------------------------------------------------

}  // namespace genie_app::transcode_fastq

// -----------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_TRANSCODE_FASTQ_PROGRAM_OPTIONS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
