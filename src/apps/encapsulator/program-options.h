/**
 * @file
 * @copyright This file is part of Genie. See LICENSE for more details.
 */

#ifndef ENCAPSULATOR_PROGRAM_OPTIONS_H_
#define ENCAPSULATOR_PROGRAM_OPTIONS_H_

#include <string>

namespace encapsulator {

class ProgramOptions {
   public:
    ProgramOptions() = delete;
    ProgramOptions(int argc, char *argv[]);
    ProgramOptions(const ProgramOptions &) = delete;
    ProgramOptions &operator=(const ProgramOptions &) = delete;
    ProgramOptions(ProgramOptions &&) = delete;
    ProgramOptions &operator=(ProgramOptions &&) = delete;
    ~ProgramOptions() = default;

    bool force;
    bool help;
    std::string inputFilePath;

   private:
    void processCommandLine(int argc, char *argv[]);
};

}  // namespace encapsulator

#endif  // ENCAPSULATOR_PROGRAM_OPTIONS_H_
