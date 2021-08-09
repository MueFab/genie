/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_GENIE_CONFORMANCE_PROGRAM_OPTIONS_H_
#define SRC_APPS_GENIE_CONFORMANCE_PROGRAM_OPTIONS_H_

#include <filesystem/filesystem.hpp>
#include <string>

namespace genieapp {
namespace conformance {

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
    std::string inputFolder;     //!< @brief

    bool help;  //!< @brief


    ghc::filesystem::path getDirName(int i) const;
    ghc::filesystem::path getFilePath(int i) const;

 private:

    /**
     * @brief
     */
    void validate() const;
};

}
}

#endif  // SRC_APPS_GENIE_CAPSULATOR_PROGRAM_OPTIONS_H_
