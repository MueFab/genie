/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_APPS_GENIE_TRANSCODE_SAM_PROGRAM_OPTIONS_H_
#define SRC_APPS_GENIE_TRANSCODE_SAM_PROGRAM_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <string>
#include <thread> //NOLINT
#include "format/sam/sam_parameter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp::transcode_sam {

/**
 * @brief
 */
class ProgramOptions {
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
    ProgramOptions(int argc, char *argv[]);

    ProgramOptions()
        : verbosity_level(0),
          tmp_dir_path("/tmp"),
          forceOverwrite(false),
          help(false),
          no_ref(false),
          clean(false),
          num_threads(std::thread::hardware_concurrency()) {}

    /**
     * @brief
     */
    ~ProgramOptions();

 public:
    int verbosity_level;          //!< @brief
    std::string tmp_dir_path;     //!< @brief
    std::string fasta_file_path;  //!< @brief
    std::string inputFile;        //!< @brief
    std::string outputFile;       //!< @brief
    bool forceOverwrite;          //!< @brief
    bool help;                    //!< @brief
    bool no_ref;                  //!< @brief
    bool clean;                   //!< @brief
    uint32_t num_threads;         //!< @brief

    [[nodiscard]] genie::format::sam::Config toConfig() const;

 private:
    /**
     * @brief
     * @param argc
     * @param argv
     */
    void processCommandLine(int argc, char *argv[]);

    /**
     * @brief
     */
    void validate();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genieapp::transcode_sam

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_TRANSCODE_SAM_PROGRAM_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
