// Copyright 2018 The genie authors


/**
 *  @file ProgramOptions.h
 *  @brief Program options implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "ProgramOptions.h"

#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

#include <iostream>
#include <set>
#include <map>

#include "common/exceptions.h"
#include "common/utilities.h"
#include "input/FileReader.h"


namespace dsg {


static void parseConfigurationFile(
    const std::string& path,
    std::vector<std::string> * const args)
{
    namespace po = boost::program_options;

    std::cout << "Parsing configuration file" << std::endl;

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
      inputFilePath(""),
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
    std::cout << "Program options:" << std::endl;
    std::cout << "  generic:" << std::endl;
    std::cout << "    force              : ";
    std::cout << ((force == true) ? "true" : "false") << std::endl;
    std::cout << "    verbose            : ";
    std::cout << (verbose ? "true" : "false") << std::endl;
    std::cout << "  input:" << std::endl;
    std::cout << "    input file path    : ";
    std::cout << inputFilePath << std::endl;
    std::cout << "    input file type    : ";
    std::cout << inputFileType << std::endl;
    std::cout << "  algorithm:" << std::endl;
    std::cout << "    read algorithm     : ";
    std::cout << readAlgorithm << std::endl;
    std::cout << "    ID algorithm       : ";
    std::cout << idAlgorithm << std::endl;
    std::cout << "    QV algorithm       : ";
    std::cout << qvAlgorithm << std::endl;
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
            "Working directory");

    // Add the input options.
    inputOptions.add_options()
        ("input-file-path,i",
            po::value<std::string>(&inputFilePath)->required(),
            "Input file path")
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
    po::store(po::parse_command_line(argc, argv, basicOptions), basicOptionsMap);

    // Now check if we shall load the other options from a configuration file.
    if (basicOptionsMap.count("configuration-file-path")) {
        const std::string& configurationFilePath = basicOptionsMap["configuration-file-path"].as<std::string>();
        std::vector<std::string> args;
        parseConfigurationFile(configurationFilePath, &args);
        po::store(po::command_line_parser(args).options(allOptions).run(), allOptionsMap);
    }

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
            std::cout << "Unknown option group name ";
            std::cout << "'" << selectedOptionGroup << "'";
            std::cout << std::endl;

            std::cout << "Allowed option group names are:" << std::endl;
            for (const auto& allowedOptionGroupName : allowedOptionGroupNames) {
                std::cout << "  " << allowedOptionGroupName << std::endl;
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


    //
    // inputFileType
    //

    std::set<std::string> allowedInputFileTypes = {
        "FASTA",
        "FASTQ",
        "SAM"
    };

    // Check if the user input string for inputFileType is in the set of
    // allowed input file types.
    it = allowedInputFileTypes.find(inputFileType);

    if (it == allowedInputFileTypes.end()) {
        std::cout << "Input file type ";
        std::cout << "'" << inputFileType << "' is invalid";
        std::cout << std::endl;

        std::cout << "Allowed input file types are:" << std::endl;
        for (const auto& allowedInputFileType : allowedInputFileTypes) {
            std::cout << "  " << allowedInputFileType << std::endl;
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
        std::cout << "ID algorithm ";
        std::cout << "'" << idAlgorithm << "' is invalid";
        std::cout << std::endl;

        std::cout << "Allowed ID algorithms are:" << std::endl;
        for (const auto& allowedIdAlgorithm : allowedIdAlgorithms) {
            std::cout << "  " << allowedIdAlgorithm << std::endl;
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
        std::cout << "QV  algorithm ";
        std::cout << "'" << qvAlgorithm << "' is invalid";
        std::cout << std::endl;

        std::cout << "Allowed QV algorithms are:" << std::endl;
        for (const auto& allowedQvAlgorithm : allowedQvAlgorithms) {
            std::cout << "  " << allowedQvAlgorithm << std::endl;
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
        std::cout << "Read algorithm ";
        std::cout << "'" << readAlgorithm << "' is invalid";
        std::cout << std::endl;

        std::cout << "Allowed read algorithms are:" << std::endl;
        for (const auto& allowedReadAlgorithm : allowedReadAlgorithms) {
            std::cout << "  " << allowedReadAlgorithm << std::endl;
        }
        throwRuntimeError("read algorithm is invalid");
    }
}


void ProgramOptions::validateDependencies(void)
{
    if (readAlgorithm == "HARC") {
        if ((inputFileType != "FASTA") && (inputFileType != "FASTQ")) {
            std::cout << "Read algorithm 'HARC' requires a FASTA file "
                         "or a FASTQ file as input" << std::endl;
            throwRuntimeError("invalid dependency");
        }
    }

    if (readAlgorithm == "TSC") {
        if ((inputFileType != "SAM")) {
            std::cout << "Read algorithm 'TSC' requires a SAM file "
                         "as input" << std::endl;
            throwRuntimeError("invalid dependency");
        }
    }

    if (idAlgorithm == "TOK") {
        // Nothing to do here.
    }

    if (qvAlgorithm == "CALQ") {
        if ((inputFileType != "SAM")) {
            std::cout << "QV algorithm 'CALQ' requires a SAM file "
                         "as input" << std::endl;
            throwRuntimeError("invalid dependency");
        }
    }

    if (qvAlgorithm == "QVZ") {
        // Nothing to do here.
    }
}


}  // namespace dsg

