/**
 * Copyright 2018-2024 The Genie Authors.
 * @file sam_parameter.h
 *
 * @brief Defines configuration parameters and options for handling SAM format
 * processing in Genie.
 *
 * This file is part of the Genie project and provides the `Config` class, which
 * encapsulates parameters and settings required for processing SAM (Sequence
 * Alignment/Map) files. The configuration includes paths, flags, and system
 * settings necessary for SAM file operations.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_SAM_PARAMETER_H_
#define SRC_GENIE_FORMAT_SAM_SAM_PARAMETER_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <string>
#include <thread>  //NOLINT

// -----------------------------------------------------------------------------

namespace genie::format::sam {

/**
 * @brief Configuration parameters for handling SAM format processing.
 */
class Config {
 public:
  /**
   * @brief Represents the configuration settings for the application.
   */
  Config()
      : tmp_dir_path_("/tmp"),
        clean_(false),
        num_threads_(std::thread::hardware_concurrency()) {}

  /// Path to the temporary directory.
  std::string tmp_dir_path_;

  /// Path to the reference FASTA file.
  std::string fasta_file_path_;

  /// Path to the input SAM file.
  std::string input_file_;

  /// Whether to clean unsupported records
  bool clean_;

  /// How many threads to use for phase 1 transcoding
  uint32_t num_threads_;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_SAM_PARAMETER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
