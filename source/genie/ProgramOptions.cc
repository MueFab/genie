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


static void printHelp(
    const boost::program_options::options_description& optionsDescription)
{
    std::cout << optionsDescription;
}


ProgramOptions::ProgramOptions(
    int argc,
    char *argv[])
    : force(false),
      verbose(false),
      workingDirectory(""),
      numThreads(1),
      inputFilePath(""),
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
    po::options_description basicOptions("Basic");
    po::options_description genericOptions("Generic");
    po::options_description inputOptions("Input");
    po::options_description algorithmOptions("Algorithm");

    std::set<std::string> allowedOptionGroupNames = {
        "basic",
        "generic",
        "input",
        "algorithm"
    };

    // Add the basic options.
    // These go without a member variable and are processed directly here in
    // this function.
    basicOptions.add_options()
        ("configuration-file-path,c",
            po::value<std::string>(),
            "Configuration file path")
        ("help,h",
            "Print help")
        ("help-group",
            po::value<std::string>(),
            "Print help for given option group only");

    // Add the generic options.
    genericOptions.add_options()
        ("force",
         po::bool_switch(&force),
            "Overwrite output files")
        ("verbose,v",
            po::bool_switch(&verbose),
            "Be verbose")
        ("working-directory,d",
            po::value<std::string>(&workingDirectory)->required(),
            "Working directory")
        ("num-thr",
            po::value<int>(&numThreads),
            "Number of threads");

    // Add the input options.
    inputOptions.add_options()
        ("input-file-path,i",
            po::value<std::string>(&inputFilePath)->required(),
            "Input file path")
        ("input-file-pair-path,p",
            po::value<std::string>(&inputFilePairPath),
            "Input file pair path")
        ("input-file-type,t",
            po::value<std::string>(&inputFileType)->required(),
            "Input file type");

    // Add the algorithm options.
    algorithmOptions.add_options()
        ("read-algorithm",
            po::value<std::string>(&readAlgorithm),
            "Read algorithm")
        ("id-algorithm",
            po::value<std::string>(&idAlgorithm),
            "ID algorithm")
        ("qv-algorithm",
            po::value<std::string>(&qvAlgorithm),
            "QV algorithm");

    // Declare an options_description instance which will include all the
    // options except for the basic options.
    po::options_description allOptions("All");
    allOptions.add(genericOptions);
    allOptions.add(inputOptions);
    allOptions.add(algorithmOptions);

    // Declare to maps for the options.
    po::variables_map basicOptionsMap;
    po::variables_map allOptionsMap;

    // Parse only the basic options from the command line.
    po::store(po::command_line_parser(argc, argv).options(basicOptions).allow_unregistered().run(), basicOptionsMap);

    // Now check if we shall load the other options from a configuration file.
    if (basicOptionsMap.count("configuration-file-path")) {
        const std::string& configurationFilePath = basicOptionsMap["configuration-file-path"].as<std::string>();
        std::vector<std::string> args;
        parseConfigurationFile(configurationFilePath, &args);
        po::store(po::command_line_parser(args).options(allOptions).run(), allOptionsMap);
    }
    else
        po::store(po::parse_command_line(argc, argv, allOptions), allOptionsMap);

    // First thing to do is to print the help.
    if (basicOptionsMap.count("help") || basicOptionsMap.count("h")) {
        printHelp(basicOptions);
        printHelp(allOptions);
        return;
    }

    // Now check if we are to print the help for a given option group only.
    if (basicOptionsMap.count("help-group")) {
        const std::string& selectedOptionGroup = basicOptionsMap["help-group"].as<std::string>();

        if (selectedOptionGroup == "basic") {
            printHelp(basicOptions);
        } else if (selectedOptionGroup == "generic") {
            printHelp(genericOptions);
        } else if (selectedOptionGroup == "input") {
            printHelp(inputOptions);
        } else if (selectedOptionGroup == "algorithm") {
            printHelp(allOptions);
        } else {
            // LOG_S(ERROR) << "Unknown option group name '" << selectedOptionGroup << "'";
            // LOG_S(ERROR) << "Allowed option group names are:";
            for (const auto& allowedOptionGroupName : allowedOptionGroupNames) {
                // LOG_S(ERROR) << "  " << allowedOptionGroupName;
            }
            throwRuntimeError("unknown option group name");
        }
        return;
    }

    // This will throw on erroneous program options. Thus we call notify()
    // after printing the help.
    po::notify(basicOptionsMap);
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
    }

    //
    // inputFileType
    //

    std::set<std::string> allowedInputFileTypes = {
        "FASTA",
        "FASTQ",
        "SAM"
    };

#ifndef GENIE_USE_OPENMP
    if (numThreads != 1)
    {
        throwRuntimeError("invalid options: numThreads != 1 && !GENIE_USE_OPENMP");
    }
#endif

    // Check if the user input string for inputFileType is in the set of
    // allowed input file types.
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
    if (readAlgorithm == "HARC") {
        if ((inputFileType != "FASTA") && (inputFileType != "FASTQ")) {
            // LOG_S(ERROR) << "Read algorithm 'HARC' requires a FASTA file "
                         // "or a FASTQ file as input";
            throwRuntimeError("invalid dependency");
        }
    }

    if (readAlgorithm == "TSC") {
        if ((inputFileType != "SAM")) {
            // LOG_S(ERROR) << "Read algorithm 'TSC' requires a SAM file "
                         // "as input";
            throwRuntimeError("invalid dependency");
        }
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
