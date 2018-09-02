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


struct ProgramOptions {
    ProgramOptions(void);

    ~ProgramOptions(void);

    void print(void);

    void validate(void);

    bool force;
    std::string inputFileName;
    std::string inputPairFileName;
    std::string outputFileName;
    std::string inputFileType;
    int numThr;
    bool verbose;
};


}  // namespace dsg


#endif  // DSG_PROGRAMOPTIONS_H_

