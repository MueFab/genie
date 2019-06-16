#ifndef GENIE_PROGRAM_OPTIONS_H_
#define GENIE_PROGRAM_OPTIONS_H_


#include <string>
#include <vector>


namespace genie {


class ProgramOptions {
public:
    ProgramOptions(int argc, char *argv[]);
    ~ProgramOptions();
public:
    std::string inputFilePath;
private:
    void processCommandLine(int argc, char *argv[]);
};


}  // namespace genie


#endif  // GENIE_PROGRAM_OPTIONS_H_
