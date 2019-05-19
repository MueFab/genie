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

#include "genie/generation.h"
#include "genie/ProgramOptions.h"
#include "genie/exceptions.h"


static void printProgramInformation(void);
static void loggerInitialization(
    int argc,
    char *argv[]);

static int dsg_main(
    int argc,
    char *argv[])
{
#ifdef GENIE_USE_OPENMP
    std::cout << "Compiled with -DGENIE_USE_OPENMP" << std::endl;
#else
    std::cout << "*Not* using OpenMP" << std::endl;
#endif

    try {
        loggerInitialization(argc, argv);
        printProgramInformation();

        dsg::ProgramOptions programOptions(argc, argv);

        if(programOptions.help) {
            return 0;
        }

        if(programOptions.inputFileType != "GENIE" && programOptions.inputFileType != "SGENIE") {
            generation(programOptions);
        } else {
            decompression(programOptions);
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
    catch (const dsg::common::RuntimeErrorException& e) {
        std::cerr << "Runtime error";
        if (strlen(e.what()) > 0) {
            std::cerr << ": " << e.what();
        }
        std::cerr << std::endl;
        return -1;
    }
    catch (const std::exception& e) {
        std::cerr << "STL error: " << e.what() << std::endl;
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

    // Exit to the operating system.
    std::cerr << "Exiting with return code: ";
    std::cerr << ((rc == 0) ? EXIT_SUCCESS : EXIT_FAILURE);
    std::cerr << std::endl;
    return (rc == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void loggerInitialization(
    int argc,
    char *argv[])
{
    // loguru::init(argc, argv);
    //
    // // Initialize logs for syslog protocol severity levels
    // loguru::add_file("logs/full_emergency.log",     loguru::Append, loguru::Verbosity_EMERGENCY);
    // loguru::add_file("logs/full_alert.log",         loguru::Append, loguru::Verbosity_ALERT);
    // loguru::add_file("logs/full_critical.log",      loguru::Append, loguru::Verbosity_CRITICAL);
    // loguru::add_file("logs/full_error.log",         loguru::Append, loguru::Verbosity_ERROR);
    // loguru::add_file("logs/full_warning.log",       loguru::Append, loguru::Verbosity_WARNING);
    // loguru::add_file("logs/full_notice.log",        loguru::Append, loguru::Verbosity_NOTICE);
    // loguru::add_file("logs/full_informatonal.log",  loguru::Append, loguru::Verbosity_INFORMATIONAL);
    // loguru::add_file("logs/full_debug.log",         loguru::Append, loguru::Verbosity_DEBUG);
    //
    // loguru::add_file("logs/latest_emergency.log",       loguru::Truncate, loguru::Verbosity_EMERGENCY);
    // loguru::add_file("logs/latest_alert.log",           loguru::Truncate, loguru::Verbosity_ALERT);
    // loguru::add_file("logs/latest_critical.log",        loguru::Truncate, loguru::Verbosity_CRITICAL);
    // loguru::add_file("logs/latest_error.log",           loguru::Truncate, loguru::Verbosity_ERROR);
    // loguru::add_file("logs/latest_warning.log",         loguru::Truncate, loguru::Verbosity_WARNING);
    // loguru::add_file("logs/latest_notice.log",          loguru::Truncate, loguru::Verbosity_NOTICE);
    // loguru::add_file("logs/latest_informatonal.log",    loguru::Truncate, loguru::Verbosity_INFORMATIONAL);
    // loguru::add_file("logs/latest_debug.log",           loguru::Truncate, loguru::Verbosity_DEBUG);
    //
    // LOG_F(ALERT, "Alert");
    // LOG_F(CRITICAL, "Critical");
    // LOG_F(ERROR, "Error");
    // LOG_F(WARNING, "Warning");
    // LOG_F(NOTICE, "Notice");
    // LOG_F(INFORMATIONAL, "Informational");
    // LOG_F(DEBUG, "Debug");
    //
    // // Only show most relevant things on stderr:
    // loguru::g_stderr_verbosity = loguru::Verbosity_DEBUG;
}


static void printProgramInformation(void)
{
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "genie dsg (descriptor stream generator)" << std::endl;
    std::cout << "Copyright (c) 2018 The genie authors" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
}
