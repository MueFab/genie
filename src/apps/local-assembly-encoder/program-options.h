#ifndef LAE_PROGRAM_OPTIONS_H_
#define LAE_PROGRAM_OPTIONS_H_

#include <string>
#include <format/mpegg_rec/mpegg-record.h>

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


   private:
    void processCommandLine(int argc, char *argv[]);
};

}  // namespace lae

#endif  // LAE_PROGRAM_OPTIONS_H_
