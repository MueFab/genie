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


namespace dsg {


class ProgramOptions {
 public:
    ProgramOptions(
        int argc,
        char *argv[]);

    ~ProgramOptions(void);

    void print(void);

 public:
    // Generic
    bool force;
    bool verbose;
    bool help;
    bool analyze;
    bool gabacDebug;
    bool discard_quality;
    bool discard_ids;
    bool preserve_order;
    bool combine_pairs;
    std::string workingDirectory;
    int numThreads;

    // Input
    std::string inputFilePath;
    std::string configPath;
    std::string inputFilePairPath;
    std::string inputFileType;

    // Output
    std::string outputFilePath;

    // Algorithm
    std::string idAlgorithm;
    std::string qvAlgorithm;
    std::string readAlgorithm;

 private:
    void processCommandLine(
        int argc,
        char *argv[]);

    void validate(void);

    void validateDependencies(void);
};


}  // namespace dsg


#endif  // DSG_PROGRAMOPTIONS_H_

