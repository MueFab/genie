/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_SAM_PARAMETER_H_
#define SRC_GENIE_FORMAT_SAM_SAM_PARAMETER_H_

// -----------------------------------------------------------------------------

#include <genie/core/record/record.h>

#include <cstdint>
#include <string>
#include <thread>  //NOLINT

// -----------------------------------------------------------------------------

namespace genie::format::sam {

/**
 * @brief
 */
class Config {
 public:
  /**
   * @brief
   */
  enum Format : std::uint8_t {
    kSam = 0,
  };

  /**
   * @brief
   */
  Config()
      : tmp_dir_path_("/tmp"),
        force_overwrite_(false),
        no_ref_(false),
        clean_(false),
        num_threads_(std::thread::hardware_concurrency()) {}

  /**
   * @brief
   */
  ~Config();

  std::string tmp_dir_path_;     //!< @brief
  std::string fasta_file_path_;  //!< @brief
  std::string input_file_;       //!< @brief
  std::string output_file_;      //!< @brief
  bool force_overwrite_;         //!< @brief
  bool no_ref_;                  //!< @brief
  bool clean_;                   //!< @brief
  uint32_t num_threads_;         //!< @brief
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_SAM_PARAMETER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
