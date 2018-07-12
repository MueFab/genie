// Copyright 2018 The genie authors


/**
 *  @file dsg.cc
 *  @brief dsg (descriptor stream generator) main file
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include <stdlib.h>
#include <string.h>

#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

#include <iostream>
#include <fstream>

#include "generation.h"
#include "ProgramOptions.h"
#include "common/exceptions.h"


static void printHelp(
    const boost::program_options::options_description& optionsDescription);


static void printVersionAndCopyright(void);


static void processProgramOptions(
    int argc,
    char *argv[],
    dsg::ProgramOptions * const programOptions);


static int dsg_main(
    int argc,
    char *argv[])
{
    try {
        printVersionAndCopyright();

        dsg::ProgramOptions programOptions;

        processProgramOptions(argc, argv, &programOptions);

        // generation(programOptions);
    }
    catch(boost::program_options::error& e) {
        std::cerr << "Program options error";
        if (strlen(e.what()) > 0) {
            std::cerr << ": " << e.what();
        }
        std::cerr << std::endl;
        return -1;
    }
    catch (const dsg::common::RuntimeErrorException& e) {
        std::cerr << "Runtime error";
        if (strlen(e.what()) > 0) {
            std::cerr << ": " << e.what();
        }
        std::cerr << std::endl;
        return -1;
    }
    catch (...) {
        std::cerr << "Unkown error occurred" << std::endl;
        return -1;
    }

    return 0;
}


/**
 *  @brief Main function
 *  @param argc Argument count
 *  @param argv Argument values
 *  @return EXIT_FAILURE on failure and EXIT_SUCCESS on success.
 */
int main(
    int argc,
    char *argv[])
{
    int rc = dsg_main(argc, argv);
    if (rc != 0) {
        std::cerr << "Failed to run dsg" << std::endl;
    }

    // The C standard makes no guarantees as to when output to stdout or stderr
    // (standard error) is actually flushed. If e.g. stdout is directed to a
    // file and an error occurs while flushing the data (after program
    // termination), then the output may be lost.
    // Thus we explicitly flush stdout and stderr. On failure, we notify the OS
    // by returning with EXIT_FAILURE.
    if (fflush(stdout) == EOF) {
        return EXIT_FAILURE;
    }
    if (fflush(stderr) == EOF) {
        return EXIT_FAILURE;
    }

    // Exit.
    std::cerr << "Exiting with return code: " << ((rc == 0) ? EXIT_SUCCESS : EXIT_FAILURE) << std::endl;
    return (rc == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


static void printHelp(
    const boost::program_options::options_description& optionsDescription)
{
    std::cout << optionsDescription;
}


static void printVersionAndCopyright(void)
{
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "genie dsg (descriptor stream generator)" << std::endl;
    std::cout << "Copyright (c) 2018 The genie authors" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
}


static void processProgramOptions(
    int argc,
    char *argv[],
    dsg::ProgramOptions * const programOptions)
{
    namespace po = boost::program_options;

    // TO-DO (Florian Baumgarte): example/real check for conflicting options
    // TO-DO (Florian Baumgarte): multiple input files -> conflicting
    // TO-DO (Florian Baumgarte): support for each input file - what do I need for which?
    // TO-DO (Florian Baumgarte): grouping options -> easy

    // Declare the supported program option groups.
    po::options_description optionsGeneric("Generic-Options");
    po::options_description optionsInput("Input-Options");
    po::options_description optionsOutput("Output-Options");
    po::options_description optionsAlgorithm("Algorithm-Options");

    optionsGeneric.add_options()
        ("rootdir,d",
            po::value<std::string>(&(programOptions->root))->required(),
            "Project root directory")
        ("force,f",
            po::bool_switch(&(programOptions->force)),
            "Overwrite output files")
        ("help,h",
            "Print help")
        ("help-module", po::value<std::string>(),
            "Print help for given module")
        ("version",
            "Output the version number")
        ("verbose,v",
            po::bool_switch(&(programOptions->verbose)),
            "Be verbose")
        ("config-file", po::value<std::string>(),
            "Config file")
    ;
    
    optionsInput.add_options()
        ("input-file-name,i",
            po::value<std::string>(&(programOptions->inputFileName))->required(),
            "Input file name")
        ("input-file-type,t",
            po::value<std::string>(&(programOptions->inputFileType))->required(),
            "Input file type")
    ;
    
    optionsOutput.add_options()
        ("output-file-base-name,o",
            po::value<std::string>(&(programOptions->outputFileName))->required(),
            "Output files base name")
    ;

    optionsAlgorithm.add_options()
        ("read-algorithm,r",
            po::value<std::string>(&(programOptions->readAlgo))->required(),
            "read-algorithm to use")
        ("id-compression-algorithm,x",
            po::value<std::string>(&(programOptions->idCompAlgo))->required(),
            "ID-compression-algorithm to use")
        ("qv-compression-algorithm,y",
            po::value<std::string>(&(programOptions->qvCompAlgo))->required(),
            "qv-compression-algorithm to use")
    ;

    // Declare an options description instance which will include
    // all the options
    po::options_description all("Allowed options");
    all.add(optionsGeneric).add(optionsInput).add(optionsOutput).add(optionsAlgorithm);

    // Parse the command line.
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, all), vm);

    // First thing to do is to print the help.
    if (vm.count("help") || vm.count("h")) {
        printHelp(all);
        return;
    }
    if (vm.count("help-module")) {
        const std::string &s = vm["help-module"].as<std::string>();
        if (s == "generic") {
            std::cout << optionsGeneric;
        } else if (s == "input") {
            std::cout << optionsInput;
        } else if (s == "output") {
            std::cout << optionsOutput;
        } else if (s == "algorithm") {
            std::cout << optionsAlgorithm;
        } else {
            std::cout << "Unknown module '" << s << "' in the --help-module option\n";
            throwRuntimeError("Unknown module in the --help-module option");
        }
        return;
    }
    if (vm.count("config-file")) {
        // load file
        std::ifstream ifs(vm["config-file"].as<std::string>().c_str());
        if (!ifs) {
            std::cout << "Could not open the config file." << std::endl;
            throwRuntimeError("Failed to open file.");
        }
        // Read file into stringstream
        std::stringstream ss;
        ss << ifs.rdbuf();
        // Split file contents
        boost::char_separator<char> sep(" \n\r");
        std::string sstr = ss.str();
        boost::tokenizer<boost::char_separator<char> > tok(sstr, sep);
        std::vector<std::string> args;
        std::copy(tok.begin(), tok.end(), std::back_inserter(args));
        store(po::command_line_parser(args).options(all).run(), vm);
    }

    // This will throw on erroneous program options. Thus, we call notify()
    // after printing the help.
    po::notify(vm);

    // Validate and print the program options.
    programOptions->print();
    programOptions->validate();
}

