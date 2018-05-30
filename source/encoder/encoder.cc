// Copyright 2018 The genie authors

/**
 *  @file encoder.cc
 *  @brief encoder main file
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdexcept>


static int encoder_main(int argc, char *argv[])
{
    try {
        fprintf(stdout, "encoder\n");
    }
    catch (const std::runtime_error& e) {
        fprintf(stderr, "Error: %s\n", e.what());
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
    int rc = encoder_main(argc, argv);
    if (rc != 0) {
        fprintf(stderr, "Error: Failed to run encoder\n");
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
