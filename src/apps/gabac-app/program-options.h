#ifndef GABACIFY_PROGRAM_OPTIONS_H_
#define GABACIFY_PROGRAM_OPTIONS_H_

#include <string>

namespace gabacify {

class ProgramOptions {
   public:
    ProgramOptions(int argc, char *argv[]);

    ~ProgramOptions();

   public:
    std::string logLevel;
    std::string inputFilePath;
    std::string dependencyFilePath;
    std::string outputFilePath;
    std::string task;
    size_t blocksize;

    uint8_t descID;
    uint8_t subseqID;

   private:
    void processCommandLine(int argc, char *argv[]);

    void validate();
};

}  // namespace gabacify

#endif  // GABACIFY_PROGRAM_OPTIONS_H_
