// Copyright 2018 The genie authors

/**
 *  @file ProgramOptions.h
 *  @brief Program options interface
 *  @author Jan Voges
 *  @bug No known bugs
 */

#ifndef DSG_PROGRAMOPTIONS_H_
#define DSG_PROGRAMOPTIONS_H_

#include <string>

namespace genieapp {
namespace run {

class ProgramOptions {
   public:
    ProgramOptions(int argc, char *argv[]);

   public:
    std::string inputFile;
    std::string inputSupFile;
    std::string inputRefFile;

    std::string outputFile;
    std::string outputSupFile;
    std::string outputRefFile;

    std::string workingDirectory;

    std::string paramsetPath;

    std::string qvMode;
    std::string readNameMode;

    bool forceOverwrite;

    bool lowLatency;
    bool externalRef;

    size_t numberOfThreads;

   private:
    void validate();
};

}  // namespace run
}  // namespace genieapp

#endif  // DSG_PROGRAMOPTIONS_H_
