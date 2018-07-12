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
#include <map>

#include "common/exceptions.h"
#include "common/utilities.h"


namespace dsg {


ProgramOptions::ProgramOptions(void)
    : root(""),
      force(false),
      verbose(false),
      inputFileName(""),
      inputFileType(""),
      outputFileName(""),
      readAlgo(""),
      idCompAlgo(""),
      qvCompAlgo("")
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
    std::cout << "  Generic:" << std::endl;
    std::cout << "    project root             : " << root << std::endl;
    std::cout << "    force                    : " << ((force == true) ? "true" : "false") << std::endl;
    std::cout << "    verbose                  : " << (verbose ? "true" : "false") << std::endl;
    std::cout << "  Input:" << std::endl;
    std::cout << "    input file name          : " << inputFileName << std::endl;
    std::cout << "    input file type          : " << inputFileType << std::endl;
    std::cout << "  Output:" << std::endl;
    std::cout << "    output file name         : " << outputFileName << std::endl;
    std::cout << "  algorithm:" << std::endl;
    std::cout << "    Read algorithm           : " << readAlgo << std::endl;
    std::cout << "    ID compression algorithm : " << idCompAlgo << std::endl;
    std::cout << "    QV compression algorithm : " << qvCompAlgo << std::endl;
}


bool checkAlgorithmInputFile(std::map<std::string, std::set<std::string>> mapAlgoType,
                                std::string algo, std::string inputFileType)
{
    if(mapAlgoType[algo].find(inputFileType) == mapAlgoType[algo].end()) {
        std::cout << "Input file type '" << inputFileType << "' is invalid for this algorithm." << std::endl;
        std::cout << "Allowed types for " << algo << ":" << std::endl;
        for (const auto& allowedType : mapAlgoType[algo]) {
            std::cout << "  " << allowedType << std::endl;
        }
        return false;
    }
    return true;
}

void ProgramOptions::validate(void)
{
    //
    // force
    //


    //
    // inputFileName
    //

    if (inputFileName.empty() == true) {
        throwRuntimeError("no input file name provided");
    }

    if (common::fileExists(inputFileName) == false) {
        throwRuntimeError("input file does not exist");
    }


    //
    // inputFileType
    //

    std::set<std::string> allowedInputFileTypes = {"FASTA", "FASTQ", "SAM"};

    // Check if the user input string for input file type is in the set of
    // allowed input file types.
    std::set<std::string>::iterator it = allowedInputFileTypes.find(inputFileType);

    if (it == allowedInputFileTypes.end()) {
        std::cout << "Input file type '" << inputFileType << "' is invalid" << std::endl;
        std::cout << "Allowed input file types are:" << std::endl;
        for (const auto& allowedInputFileType : allowedInputFileTypes) {
            std::cout << "  " << allowedInputFileType << std::endl;
        }
        throwRuntimeError("input file type is invalid");
    }


    //
    // outputFileName
    //

    if (outputFileName.empty() == true) {
        throwRuntimeError("no output file name provided");
    }

    if ((common::fileExists(outputFileName)) == true && (force == false)) {
        throwRuntimeError("not overwriting output file (use option 'f' to force overwriting)");
    }


    //
    // verbose
    //

    if (verbose == true) {
        std::cout << "Verbose log output activated" << std::endl;
    }

    //
    // sanity check for algorithm input types
    //
    std::map<std::string, std::set<std::string>> mapAlgoType;
    // build one set of allowed input-files for every algorithm
    std::set<std::string> calqAllowed;
    calqAllowed.insert("SAM");
    mapAlgoType["CALQ"] = calqAllowed;

    if (!checkAlgorithmInputFile(mapAlgoType, qvCompAlgo, inputFileType)) {
        throwRuntimeError("Input file invalid for chosen algorithm.");
    }




    std::cout << "Program options are valid" << std::endl;
}


}  // namespace dsg

