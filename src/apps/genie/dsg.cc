// Copyright 2018 The genie authors

/**
 *  @file dsg.cc
 *  @brief dsg (descriptor stream generator) main file
 *  @author Jan Voges
 *  @bug No known bugs
 */

#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "generation.h"
#include "program-options.h"
#include "ureads-encoder/logger.h"
#include "util/exceptions.h"

static void printProgramInformation();
static void loggerInitialization();

static int dsg_main(int argc, char* argv[]) {
#ifdef GENIE_USE_OPENMP
    std::cout << "genie: built with OpenMP" << std::endl;
#else
    std::cout << "genie: *not* built with OpenMP" << std::endl;
#endif

    try {
        loggerInitialization();
        printProgramInformation();

        dsg::ProgramOptions programOptions(argc, argv);

        if (programOptions.help) {
            return 0;
        }

        if (programOptions.inputFileType != "GENIE" && programOptions.inputFileType != "SGENIE") {
            generation(programOptions);
        } else {
            decompression(programOptions);
        }
    } catch (const util::RuntimeException& e) {
        std::cerr << "Runtime error";
        if (strlen(e.what()) > 0) {
            std::cerr << ": " << e.what();
        }
        std::cerr << std::endl;
        return -1;
    } catch (const std::exception& e) {
        std::cerr << "STL error: " << e.what() << std::endl;
    } catch (...) {
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
int main(int argc, char* argv[]) {
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

    // Exit to the operating system.
    std::cerr << "Exiting with return code: ";
    std::cerr << ((rc == 0) ? EXIT_SUCCESS : EXIT_FAILURE);
    std::cerr << std::endl;
    return (rc == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void loggerInitialization() { genie::Logger::init("genie.log"); }

static void printProgramInformation(void) {
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "genie dsg (descriptor stream generator)" << std::endl;
    std::cout << "Copyright (c) 2018 The genie authors" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
}
