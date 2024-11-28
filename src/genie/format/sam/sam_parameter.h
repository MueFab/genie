/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_SAM_PARAMETER_H_
#define SRC_GENIE_FORMAT_SAM_SAM_PARAMETER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/record/record.h>
#include <cstdint>
#include <string>
#include <thread>  //NOLINT

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::sam {

/**
 * @brief
 */
class Config {
 public:
    /**
     * @brief
     */
    enum format : std::uint8_t {
        sam = 0,
    };

    /**
     * @brief
     * @param argc
     * @param argv
     */

    Config()
        : verbosity_level(0),
          tmp_dir_path("/tmp"),
          forceOverwrite(false),
          no_ref(false),
          clean(false),
          num_threads(std::thread::hardware_concurrency()) {}

    /**
     * @brief
     */
    ~Config();

 public:
    int verbosity_level;          //!< @brief
    std::string tmp_dir_path;     //!< @brief
    std::string fasta_file_path;  //!< @brief
    std::string inputFile;        //!< @brief
    std::string outputFile;       //!< @brief
    bool forceOverwrite;          //!< @brief
    bool no_ref;                  //!< @brief
    bool clean;                   //!< @brief
    uint32_t num_threads;         //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::sam

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_SAM_PARAMETER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
