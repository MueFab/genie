/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef DSG_PROGRAMOPTIONS_H_
#define DSG_PROGRAMOPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace run {

/**
 *
 */
class ProgramOptions {
   public:
    /**
     *
     * @param argc
     * @param argv
     */
    ProgramOptions(int argc, char *argv[]);

   public:
    std::string inputFile;     //!<
    std::string inputSupFile;  //!<
    std::string inputRefFile;  //!<

    std::string outputFile;     //!<
    std::string outputSupFile;  //!<
    std::string outputRefFile;  //!<

    std::string workingDirectory;  //!<

    std::string paramsetPath;  //!<

    std::string qvMode;        //!<
    std::string readNameMode;  //!<

    bool forceOverwrite;  //!<

    bool lowLatency;   //!<
    std::string refMode;  //!<

    size_t numberOfThreads;  //!<

   private:
    /**
     *
     */
    void validate();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace run
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------

#endif  // DSG_PROGRAMOPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------