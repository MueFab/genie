#ifndef GENIE_PROGRAM_OPTIONS_H_
#define GENIE_PROGRAM_OPTIONS_H_

#include <string>
#include <vector>

namespace lowlatency_encoder {

class ProgramOptions {
   public:
    ProgramOptions(int argc, char *argv[]);

    ~ProgramOptions();

   public:
    std::string inputFilePath;
    std::string pairedPath;
    std::string outputFilePath;
    bool decompression;
    uint8_t num_threads;
    bool forceOverride;

   private:
    void processCommandLine(int argc, char *argv[]);
};

}  // namespace lowlatency_encoder

#endif  // GENIE_PROGRAM_OPTIONS_H_
