/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_APPS_GENIE_GABAC_PROGRAM_OPTIONS_H_
#define SRC_APPS_GENIE_GABAC_PROGRAM_OPTIONS_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <string>

// -----------------------------------------------------------------------------

namespace genie_app::gabac {

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

  /**
   * @brief
   */
  ~ProgramOptions() = default;

  bool fast_benchmark_;               //!< @brief
  std::string log_level_;             //!< @brief
  std::string input_file_path_;       //!< @brief
  std::string dependency_file_path_;  //!< @brief
  std::string output_file_path_;      //!< @brief
  std::string param_file_path_;       //!< @brief
  std::string task_;                  //!< @brief
  size_t blocksize_;                  //!< @brief

  uint8_t desc_id_;    //!< @brief
  uint8_t subseq_id_;  //!< @brief

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
  void validate() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie_app::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_GABAC_PROGRAM_OPTIONS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
