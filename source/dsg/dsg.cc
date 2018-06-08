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

#include <iostream>
#include <stdexcept>

#include "ProgramOptions.h"
#include "input/fasta/FastaFileReader.h"


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

        if (programOptions.inputFileType == "FASTA") {
            dsg::FastaFileReader fastaFileReader(programOptions.inputFileName);

            for (const auto fastaRecord : fastaFileReader.records) {
                std::cout << fastaRecord.header << "  ";
                std::cout << fastaRecord.sequence << std::endl;
            }
        } else if (programOptions.inputFileType == "FASTQ") {
            //
        } else if (programOptions.inputFileType == "SAM") {
            //
        } else {
            throw std::runtime_error("wrong input file type");
        }
    }
    catch(boost::program_options::error& e) {
        std::cerr << "Program options error";
        if (strlen(e.what()) > 0) {
            std::cerr << ": " << e.what();
        }
        std::cerr << std::endl;
        return -1;
    }
    catch (const std::runtime_error& e) {
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
int main(int argc, char *argv[])
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


static void printHelp(const boost::program_options::options_description& optionsDescription)
{
    std::cout << optionsDescription;
}


static void printVersionAndCopyright(void)
{
    std::cout << std::string(80, '-') << std::endl;
    printf("genie dsg (descriptor stream generator)\n");
    printf("Copyright (c) 2018 The genie authors\n");
    std::cout << std::string(80, '-') << std::endl;
}


static void processProgramOptions(
    int argc,
    char *argv[],
    dsg::ProgramOptions * const programOptions)
{
    namespace po = boost::program_options;

    // Declare the supported program options.
    po::options_description optionsDescription("Options");
    optionsDescription.add_options()
        ("force,f",
             po::bool_switch(&(programOptions->force)),
            "Overwrite output files")
        ("help,h",
            "Print help")
        ("input-file-name,i",
            po::value<std::string>(&(programOptions->inputFileName))->required(),
            "Input file name")
        ("input-file-type,t",
            po::value<std::string>(&(programOptions->inputFileType))->required(),
            "Input file type")
        ("output-file-name,o",
            po::value<std::string>(&(programOptions->outputFileName))->required(),
            "Output file name")
        ("verbose,v",
             po::bool_switch(&(programOptions->verbose)),
            "Be verbose");

    // Parse the command line.
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, optionsDescription), vm);

    // First thing to do is to print the help.
    if (vm.count("help")) {
        printHelp(optionsDescription);
        return;
    }

    // This will throw on erroneous program options. Thus, we call notify()
    // after printing the help.
    po::notify(vm);

    // Validate and print the program options.
    programOptions->print();
    programOptions->validate();
}

