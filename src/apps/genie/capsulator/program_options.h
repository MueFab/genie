/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_APPS_GENIE_CAPSULATOR_PROGRAM_OPTIONS_H_
#define SRC_APPS_GENIE_CAPSULATOR_PROGRAM_OPTIONS_H_

// -----------------------------------------------------------------------------

#include <string>

// -----------------------------------------------------------------------------

namespace genie_app::capsulator {

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

  std::string input_file_;  //!< @brief

  std::string output_file_;  //!< @brief

  bool force_overwrite_;  //!< @brief

  bool help_;  //!< @brief

 private:
  /**
   * @brief
   */
  void validate() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie_app::capsulator

// -----------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_CAPSULATOR_PROGRAM_OPTIONS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
