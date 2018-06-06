// Copyright 2018 The genie authors


/**
 *  @file dsg.cc
 *  @brief dsg (descriptor-stream-generator) main file
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <boost/program_options.hpp>

#include <stdexcept>


static void printVersionAndCopyright(void)
{
    printf("----------------------------------------------------------\n");
    printf("genie dsg (descriptor-stream-generator)\n");
    printf("Copyright (c) 2018 The genie authors\n");
    printf("----------------------------------------------------------\n");
}


static int dsg_main(int argc, char *argv[])
{
    try {
        printVersionAndCopyright();

//         parseOptions(argc, argv);

//         dsg::FASTAFile inputFASTAFile("test.fasta", dsg::FASTAFile::MODE_READ);
    }
    catch (const std::runtime_error& e) {
        fprintf(stderr, "Runtime error: %s\n", e.what());
        return -1;
    }
    catch (...) {
        fprintf(stderr, "Unkown error occurred\n");
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
    try {
        boost::program_options::options_description desc("Options");
        desc.add_options()
            ("help", "Provides helpful information")
            ("int", boost::program_options::value<int>()->default_value(10)->implicit_value(20), "Enter an int")
            ("string", boost::program_options::value<std::string>(), "Enter a string")
        ;
        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);

        if(vm.count("help")) {
            std::cout << desc << "\n";
        }
        if (vm.count("int")) {
            std::cout << "Input number was: " << vm["int"].as<int>() << ".\n";
        } else {
            std::cout << "No number was given.\n" << "number is: " << vm["int"].as<int>() << ".\n";
        }
        if (vm.count("string")) {
            std::cout << "String is: " << vm["string"].as<std::string>() << std::endl;
        } else {
            std::cout << "no string.\n";
        }       
    }

    catch(boost::program_options::error& e) 
    { 
      fprintf(stderr, "Runtime error: %s\n", e.what()); 
      return -1; 
    } 
    

    int rc = dsg_main(argc, argv);
    if (rc != 0) {
        fprintf(stderr, "Error: Failed to run dsg\n");
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

    // Return to the OS.
    return (rc == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
