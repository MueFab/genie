/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_APPS_GENIE_TRANSCODE_SAM_PROGRAM_OPTIONS_H_
#define SRC_APPS_GENIE_TRANSCODE_SAM_PROGRAM_OPTIONS_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <string>
#include <thread>  //NOLINT

#include "format/sam/sam_parameter.h"

// -----------------------------------------------------------------------------

namespace genie_app::transcode_sam {

/**
 * @brief
 */
class ProgramOptions {
 public:
  /**
   * @brief
   */
  enum Format : std::uint8_t {
    kSam = 0,
  };

  /**
   * @brief
   * @param argc
   * @param argv
   */
  ProgramOptions(int argc, char* argv[]);

  ProgramOptions()
      : verbosity_level_(0),
        tmp_dir_path_("/tmp"),
        force_overwrite_(false),
        help_(false),
        no_ref_(false),
        clean_(false),
        no_extended_alignment_(false),
        num_threads_(std::thread::hardware_concurrency()) {}

  /**
   * @brief
   */
  ~ProgramOptions();

  int verbosity_level_;          //!< @brief
  std::string tmp_dir_path_;     //!< @brief
  std::string fasta_file_path_;  //!< @brief
  std::string input_file_;        //!< @brief
  std::string output_file_;       //!< @brief
  bool force_overwrite_;          //!< @brief
  bool help_;                    //!< @brief
  bool no_ref_;                  //!< @brief
  bool clean_;                   //!< @brief
  bool no_extended_alignment_;   //!< @brief
  uint32_t num_threads_;         //!< @brief

  [[nodiscard]] genie::format::sam::Config ToConfig() const;

 private:
  /**
   * @brief
   * @param argc
   * @param argv
   */
  void ProcessCommandLine(int argc, char* argv[]);

  /**
   * @brief
   */
  void validate();
};

// -----------------------------------------------------------------------------

}  // namespace genie_app::transcode_sam

// -----------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_TRANSCODE_SAM_PROGRAM_OPTIONS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
