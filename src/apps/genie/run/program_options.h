/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_APPS_GENIE_RUN_PROGRAM_OPTIONS_H_
#define SRC_APPS_GENIE_RUN_PROGRAM_OPTIONS_H_

// -----------------------------------------------------------------------------

#include <string>

// -----------------------------------------------------------------------------

namespace genie_app::run {

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

  std::string input_file_;      //!< @brief
  std::string input_sup_file_;  //!< @brief
  std::string input_ref_file_;  //!< @brief

  std::string output_file_;      //!< @brief
  std::string output_sup_file_;  //!< @brief

  std::string working_directory_;  //!< @brief

  std::string paramset_path_;  //!< @brief

  std::string qv_mode_;         //!< @brief
  std::string read_name_mode_;  //!< @brief

  std::string entropy_mode_;  //!< @brief

  bool force_overwrite_;  //!< @brief

  bool combine_pairs_flag_;  //!< @brief

  bool low_latency_;      //!< @brief
  std::string ref_mode_;  //!< @brief

  size_t number_of_threads_;  //!< @brief
  bool raw_reference_;        //!< @brief
  bool raw_streams_;          //!< @brief

  bool help_;  //!< @brief

 private:
  /**
   * @brief
   */
  void validate();
};

// -----------------------------------------------------------------------------

}  // namespace genie_app::run

// -----------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_RUN_PROGRAM_OPTIONS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
