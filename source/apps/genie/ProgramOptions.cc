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

#include <cli11/CLI11.hpp>
#include <filesystem/filesystem.hpp>

#include "exceptions.h"
#include "utilities.h"
#include <utils/file-reader.h>


namespace dsg {


ProgramOptions::ProgramOptions(
    int argc,
    char *argv[])
    : force(false),
      verbose(false),
      help(false),
      analyze(false),
      workingDirectory("./"),
      numThreads(1),
      inputFilePath(""),
      configPath(""),
      inputFilePairPath(""),
      inputFileType(""),
      idAlgorithm(""),
      qvAlgorithm(""),
      readAlgorithm("")
{
    processCommandLine(argc, argv);
}


ProgramOptions::~ProgramOptions()
{
    // Nothing to do here.
}


void ProgramOptions::processCommandLine(
    int argc,
    char *argv[])
{
    CLI::App app("Genie MPEG-G reference encoder\n\n"
    "Usage fastq encoding: \n genie [fastq] [optional: paired fastq] -c [Path to gabac config] [Optional: -t number of threads]\n"
    "Usage fastq decoding: \n genie [genie file] -c [Path to gabac config] [Optional: -t number of threads]\n"
    "Usage sam encoding: \n genie [sam] [fasta reference] -c [Path to gabac config] [Optional: -t number of threads]\n"
    "Usage sam decoding: \n genie [sgenie file] -c [Path to gabac config] [Optional: -t number of threads]\n");

    std::string filename = "default";
    app.add_option("input-file", inputFilePath, "First input file - fastq in fastq mode, sam in sam mode, genie/sgenie file in decompression mode. This argument is positional (first argument) and mandatory")->mandatory(true);
    app.add_option("input-file-pair-path", inputFilePairPath, "Second input file - paired fastq in fastq mode (optional), reference fasta file in sam mode (mandatory). This argument is also positional.");
    app.add_option("-c,--config-file-path", configPath, "Path to directory with gabac configurations. Missing configuratons will be regenerated, which may take a while")->mandatory(true);

    analyze = false;
    app.add_flag("-g,--generate-configuration", analyze, "Generate a new set of configurations");

    numThreads = 1;
    app.add_option("-t,--numThreads", numThreads, "How many threads to launch for parallel execution of GABAC. Default is 1.");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        throw std::runtime_error("Command line parsing failed:" + std::to_string(app.exit(e)));
    }

    validate();
}


void ProgramOptions::validate()
{
    std::set<std::string>::iterator it;

    if (inputFilePath.empty()) {
        throwRuntimeError("no input file path provided");
    }

    if (!common::fileExists(inputFilePath)) {
        throwRuntimeError("input file does not exist");
    }

    if (!ghc::filesystem::exists(configPath) || !ghc::filesystem::is_directory(configPath)) {
        throwRuntimeError("config dir does not exist");
    }



    if (!inputFilePairPath.empty()) {
        if (!common::fileExists(inputFilePairPath)) {
            throwRuntimeError("input pair file does not exist");
        }
        if (inputFilePairPath == inputFilePath) {
            throwRuntimeError("Same file name for two files");
        }
        std::cout << "paired mode activated" << std::endl;
    }

    //
    // inputFileType
    //

    std::set<std::string> allowedInputFileTypes = {
        "GENIE",
        "SGENIE",
        "FASTQ",
        "SAM"
    };


    // Check if the user input string for inputFileType is in the set of
    // allowed input file types.

    inputFileType = inputFilePath.substr(inputFilePath.find_last_of('.') + 1, std::string::npos);

    std::transform(inputFileType.begin(), inputFileType.end(),inputFileType.begin(), ::toupper);

    if (allowedInputFileTypes.find(inputFileType) == allowedInputFileTypes.end()) {
        throwRuntimeError("input file type is invalid");
    }
}


}  // namespace dsg
