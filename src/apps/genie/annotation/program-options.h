/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_GENIE_ANNOTATION_PROGRAM_OPTIONS_H_
#define SRC_APPS_GENIE_ANNOTATION_PROGRAM_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace annotation {

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
    ~ProgramOptions() = default;

 public:
    std::string inputFile;  //!< @brief
    std::string outputFile;  //!< @brief
    bool forceOverwrite;  //!< @brief
    std::string task;     //!< @brief
    std::string codec;    //!< @brief
    bool help;            //!< @brief

 private:
    /**
     * @brief
     */
    void validate() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_ANNOTATION_PROGRAM_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
