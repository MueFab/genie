#ifndef LAE_PROGRAM_OPTIONS_H_
#define LAE_PROGRAM_OPTIONS_H_

#include <genie/core/record/record.h>
#include <string>

namespace lae {

class ProgramOptions {
   public:
    ProgramOptions(int argc, char *argv[]);

    ~ProgramOptions();

   public:
    std::string inputFilePath;
    std::string outputFilePath;
    bool decompression;
    uint8_t num_threads;
    bool forceOverride;
    bool discardQualities;
    bool discardNames;

   private:
    void processCommandLine(int argc, char *argv[]);
};

}  // namespace lae

#endif  // LAE_PROGRAM_OPTIONS_H_
