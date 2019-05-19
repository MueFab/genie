// Copyright 2018 The genie authors


/**
 *  @file ProgramOptions.h
 *  @brief Program options implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "genie/ProgramOptions.h"

#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

#include <iostream>
#include <set>
#include <map>

#include "genie/exceptions.h"
#include "genie/utilities.h"
#include "fileio/file_reader.h"


namespace dsg {


static void parseConfigurationFile(
    const std::string& path,
    std::vector<std::string> * const args)
{
    namespace po = boost::program_options;

    std::cout << "Parsing configurration file" << std::endl;

    dsg::input::FileReader fileReader(path);
    std::string options("");

    // Read configuration file contents.
    while (true) {
        // Try to read a line.
        std::string line("");
        fileReader.readLine(&line);
        if (line.empty() == true) {
            break;
        }

        // Append this line plus an extra space (for tokenization) to our
        // programOptionsString.
        options += " " + line;
    }

    // At this point the programOptionsString may not be empty.
    if (options.empty() == true) {
        throwRuntimeError("failed to parse configuration file");
    }

    // Tokenize the configuration file contents.
    boost::char_separator<char> separators(" ");
    boost::tokenizer<boost::char_separator<char>> tokenizer(options, separators);
    std::copy(tokenizer.begin(), tokenizer.end(), std::back_inserter(*args));
}


ProgramOptions::ProgramOptions(
    int argc,
    char *argv[])
    : force(false),
      verbose(false),
      help(false),
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


ProgramOptions::~ProgramOptions(void)
{
    // Nothing to do here.
}


void ProgramOptions::print(void)
{
    // LOG_S(INFORMATIONAL) << "Program options:";
    // LOG_S(INFORMATIONAL) << "  generic:";
    // LOG_S(INFORMATIONAL) << "    force                : " << ((force == true) ? "true" : "false");
    // LOG_S(INFORMATIONAL) << "    verbose              : " << (verbose ? "true" : "false");
    // LOG_S(INFORMATIONAL) << "    number of threads    : " << numThreads;
    // LOG_S(INFORMATIONAL) << "  input:";
    // LOG_S(INFORMATIONAL) << "    input file path      : " << inputFilePath;
    // LOG_S(INFORMATIONAL) << "    input file pair path : " << inputFilePairPath;
    // LOG_S(INFORMATIONAL) << "    input file type      : " << inputFileType;
    // LOG_S(INFORMATIONAL) << "  algorithm:";
    // LOG_S(INFORMATIONAL) << "    read algorithm       : " << readAlgorithm;
    // LOG_S(INFORMATIONAL) << "    ID algorithm         : " << idAlgorithm;
    // LOG_S(INFORMATIONAL) << "    QV algorithm         : " << qvAlgorithm;
}


void ProgramOptions::processCommandLine(
    int argc,
    char *argv[])
{
    namespace po = boost::program_options;

    // Declare the supported option groups.
    po::options_description allOptions("All");
    po::positional_options_description p;

    p.add("input-file", 1);
    p.add("input-file-pair-path", 1);

    std::set<std::string> allowedOptionGroupNames = {
        "basic",
        "generic",
        "input",
        "algorithm"
    };

    // Add the input options.
    allOptions.add_options()
        ("input-file",
            po::value<std::string>(&inputFilePath),
            "First input file - fastq in fastq mode, sam in sam mode, genie/sgenie file in decompression mode. This argument is positional (first argument) and mandatory")
        ("input-file-pair-path,p",
            po::value<std::string>(&inputFilePairPath),
            "Second input file - paired fastq in fastq mode (optional), reference fasta file in sam mode (mandatory). This argument is also positional.")
            ("config-file-path,c",
                    po::value<std::string>(&configPath),
                            "Path to directory with gabac configurations. Missing configuratons will be regenerated, which may take a while")
            ("numThreads,t",
             po::value<int>(&numThreads)->default_value(1),
             "How many threads to launch for parallel execution of GABAC. Default is 1.")
            ("help", "Print usage");

    // Declare an options_description instance which will include all the
    // options except for the basic options.


    // Declare to maps for the options.
    po::variables_map allOptionsMap;

    po::store(po::command_line_parser(argc, argv).
            options(allOptions).positional(p).run(), allOptionsMap);

    // First thing to do is to print the help.
    if (allOptionsMap.count("help") || allOptionsMap.count("h")) {
        std::cout
                << "Usage fastq encoding: \n genie [fastq] [optional: paired fastq] -c [Path to gabac config] [Optional: -t number of threads]"
                << std::endl;
        std::cout
                << "Usage fastq decoding: \n genie [genie file] -c [Path to gabac config] [Optional: -t number of threads]"
                << std::endl;
        std::cout
                << "Usage sam encoding: \n genie [sam] [fasta reference] -c [Path to gabac config] [Optional: -t number of threads]"
                << std::endl;
        std::cout
                << "Usage sam decoding: \n genie [sgenie file] -c [Path to gabac config] [Optional: -t number of threads]"
                << std::endl << std::endl;
        std::cout << allOptions << std::endl;
        help = true;
        return;
    }

    // This will throw on erroneous program options. Thus we call notify()
    // after printing the help.
    po::notify(allOptionsMap);

    print();
    validate();
    validateDependencies();
}


void ProgramOptions::validate(void)
{
    std::set<std::string>::iterator it;

    //
    // force
    //

    if (force == true) {
        // Nothing to do here.
    }

    //
    // inputFilePath
    //

    if (inputFilePath.empty() == true) {
        throwRuntimeError("no input file path provided");
    }

    if (common::fileExists(inputFilePath) == false) {
        throwRuntimeError("input file does not exist");
    }


    if (inputFilePairPath.empty() == false) {
        if (common::fileExists(inputFilePairPath) == false) {
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

    it = allowedInputFileTypes.find(inputFileType);

    if (it == allowedInputFileTypes.end()) {
        // LOG_S(ERROR) << "Input file type '" << inputFileType << "' is invalid";
        // LOG_S(ERROR) << "Allowed input file types are:";
        for (const auto& allowedInputFileType : allowedInputFileTypes) {
            // LOG_S(ERROR) << "  " << allowedInputFileType;
        }
        throwRuntimeError("input file type is invalid");
    }


    //
    // verbose
    //

    // Nothing to do here.


    //
    // idAlgorithm
    //

    std::set<std::string> allowedIdAlgorithms = {
        "TOK",
    };

    // Check if the user input string for idAlgorithm is in the
    // set of allowed ID algorithms.
    idAlgorithm = "TOK";
    it = allowedIdAlgorithms.find(idAlgorithm);

    if (it == allowedIdAlgorithms.end()) {
        // LOG_S(ERROR) << "ID algorithm '" << idAlgorithm << "' is invalid";
        // LOG_S(ERROR) << "Allowed ID algorithms are:";
        for (const auto& allowedIdAlgorithm : allowedIdAlgorithms) {
            // LOG_S(ERROR) << "  " << allowedIdAlgorithm;
        }
        throwRuntimeError("ID algorithm is invalid");
    }


    //
    // qvAlgorithm
    //

    std::set<std::string> allowedQvAlgorithms = {
        "CALQ",
        "QVZ"
    };

    qvAlgorithm = "QVZ";

    // Check if the user input string for qvAlgorithm is in the
    // set of allowed QV algorithms.
    it = allowedQvAlgorithms.find(qvAlgorithm);

    if (it == allowedQvAlgorithms.end()) {
        // LOG_S(ERROR) << "QV algorithm '" << qvAlgorithm << "' is invalid";
        // LOG_S(ERROR) << "Allowed QV algorithms are:";
        for (const auto& allowedQvAlgorithm : allowedQvAlgorithms) {
            // LOG_S(ERROR) << "  " << allowedQvAlgorithm;
        }
        throwRuntimeError("QV algorithm is invalid");
    }


    //
    // readCompressionAlgorithm
    //

    std::set<std::string> allowedReadAlgorithms = {
        "HARC",
        "TSC",
    };

    readAlgorithm = "HARC";
    // Check if the user input string for readAlgorithm is in the
    // set of allowed read algorithms.
    it = allowedReadAlgorithms.find(readAlgorithm);

    if (it == allowedReadAlgorithms.end()) {
        // LOG_S(ERROR) << "Read algorithm '" << readAlgorithm << "' is invalid";
        // LOG_S(ERROR) << "Allowed read algorithms are:";
        for (const auto& allowedReadAlgorithm : allowedReadAlgorithms) {
            // LOG_S(ERROR) << "  " << allowedReadAlgorithm;
        }
        throwRuntimeError("read algorithm is invalid");
    }
}


void ProgramOptions::validateDependencies(void)
{
        if ((inputFileType != "SGENIE") && (inputFileType != "GENIE") && (inputFileType != "FASTQ") && (inputFileType != "SAM")) {
            // LOG_S(ERROR) << "Read algorithm 'HARC' requires a FASTA file "
                         // "or a FASTQ file as input";
            throwRuntimeError("invalid dependency");
        }

    if (idAlgorithm == "TOK") {
        // Nothing to do here.
    }

    if (qvAlgorithm == "CALQ") {
        if ((inputFileType != "SAM")) {
            // LOG_S(ERROR) << "QV algorithm 'CALQ' requires a SAM file "
                         // "as input";
            throwRuntimeError("invalid dependency");
        }
    }

    if (qvAlgorithm == "QVZ") {
        // Nothing to do here.
    }
}


}  // namespace dsg
