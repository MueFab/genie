/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_GENIE_RUN_PROGRAM_OPTIONS_H_
#define SRC_APPS_GENIE_RUN_PROGRAM_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace run {

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
    std::string inputRefFile;  //!< @brief

    std::string outputFile;     //!< @brief
    std::string outputSupFile;  //!< @brief

    std::string workingDirectory;  //!< @brief

    std::string paramsetPath;  //!< @brief

    std::string qvMode;        //!< @brief
    std::string readNameMode;  //!< @brief

    bool forceOverwrite;  //!< @brief

    bool combinePairsFlag;  //!< @brief

    bool lowLatency;      //!< @brief
    std::string refMode;  //!< @brief

    size_t numberOfThreads;  //!< @brief
    bool rawReference;       //!< @brief

    bool help;  //!< @brief

 private:
    /**
     * @brief
     */
    void validate();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace run
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_RUN_PROGRAM_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
