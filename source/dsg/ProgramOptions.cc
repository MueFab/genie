// Copyright 2018 The genie authors


/**
 *  @file ProgramOptions.h
 *  @brief Program options implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "ProgramOptions.h"

#include <iostream>
#include <set>


namespace dsg {


ProgramOptions::ProgramOptions(void)
    : force(false),
      inputFileName(""),
      outputFileName(""),
      inputFileType(""),
      verbose(false)
{
    // Nothing to do here.
}


ProgramOptions::~ProgramOptions(void)
{
    // Nothing to do here.
}


void ProgramOptions::print(void)
{
    std::cout << "Program options:" << std::endl;
    std::cout << "  Force            : " << (force ? "true" : "false") << std::endl;
    std::cout << "  Input file name  : " << inputFileName << std::endl;
    std::cout << "  Input file type  : " << inputFileType << std::endl;
    std::cout << "  Output file name : " << outputFileName << std::endl;
    std::cout << "  Verbose          : " << (verbose ? "true" : "false") << std::endl;
}


void ProgramOptions::validate(void)
{
    std::cout << "Validating program options" << std::endl;

    // force
    if (force == true) {
        std::cout << "Attention: overwriting output files!" << std::endl;
    }

    // inputFileName
    if (inputFileName.empty() == true) {
        throw std::runtime_error("No input file name provided");
    }

//     if (fileExists(inputFileName) == false) {
//         throw std::runtime_error("Input file does not exist");
//     }

    // inputFileType
    std::set<std::string> allowedInputFileTypes{"FASTA", "FASTQ", "SAM"};

    // Check if the user input string for input file type is in the set of
    // allowed input file types.
    std::set<std::string>::iterator it = allowedInputFileTypes.find(inputFileType);

    if (it == allowedInputFileTypes.end()) {
        std::cout << "Input file type '" << inputFileType << "' is invalid" << std::endl;
        std::cout << "Allowed input file types are:" << std::endl;
        for (const auto& allowedInputFileType : allowedInputFileTypes) {
            std::cout << "  " << allowedInputFileType << std::endl;
        }
        throw std::runtime_error("Input file type is invalid");
    }

    // outputFileName
    if (outputFileName.empty() == true) {
        throw std::runtime_error("No output file name provided");
    }

//     if ((fileExists(outputFileName)) == true && (force == false)) {
//         throw std::runtime_error("Not overwriting output file (use option 'f' to force overwriting)");
//     }

    // verbose
    if (verbose == true) {
        std::cout << "Verbose log output activated" << std::endl;
    }

    std::cout << "Program options are valid" << std::endl;
}

}  // namespace dsg
