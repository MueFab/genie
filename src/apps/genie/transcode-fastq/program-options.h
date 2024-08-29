/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_GENIE_TRANSCODE_FASTQ_PROGRAM_OPTIONS_H_
#define SRC_APPS_GENIE_TRANSCODE_FASTQ_PROGRAM_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp::transcode_fastq {

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
    ProgramOptions(int argc, char *argv[]);

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

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genieapp::transcode_fastq

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_TRANSCODE_FASTQ_PROGRAM_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
