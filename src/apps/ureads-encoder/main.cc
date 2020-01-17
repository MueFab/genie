#include <cassert>
#include <csignal>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <genie/util/exceptions.h>
#include "encoding.h"
#include "program-options.h"

static void writeCommandLine(int argc, char* argv[]) {
    std::vector<std::string> args(argv, (argv + argc));
    std::stringstream commandLine;
    for (const auto& arg : args) {
        commandLine << arg << " ";
    }
    std::cout << "Command line: " << commandLine.str() << std::endl;
}

static int genie_main(int argc, char* argv[]) {
    try {
        ureads_encoder::ProgramOptions programOptions(argc, argv);
        writeCommandLine(argc, argv);
        ureads_encoder::encode(programOptions);
    } catch (const genie::util::RuntimeException& e) {
        std::cerr << "Runtime error: " << e.msg() << std::endl;
        return -1;
    } catch (const std::exception& e) {
        std::cerr << "Standard library error: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return -1;
    }

    return 0;
}

extern "C" void handleSignal(int sig) {
    // Ignore the signal
    std::signal(sig, SIG_IGN);

    // Get signal string and log it
    std::string signalString;
    switch (sig) {
        case SIGABRT:
            signalString = "SIGABRT";
            break;
        case SIGFPE:
            signalString = "SIGFPE";
            break;
        case SIGILL:
            signalString = "SIGILL";
            break;
        case SIGINT:
            signalString = "SIGINT";
            break;
        case SIGSEGV:
            signalString = "SIGSEGV";
            break;
        case SIGTERM:
            signalString = "SIGTERM";
            break;
        default:
            signalString = "unknown";
            break;
    }
    std::cerr << "Caught signal: " << sig << " (" << signalString << ")" << std::endl;

    // Invoke the default signal action
    std::signal(sig, SIG_DFL);
    std::raise(sig);
}

int main(int argc, char* argv[]) {
    // Install signal handler for the following signal types:
    //   SIGABRT  abnormal termination condition, as is e.g. initiated by std::abort()
    //   SIGFPE   erroneous arithmetic operation such as divide by zero
    //   SIGILL   invalid program image, such as invalid instruction
    //   SIGINT   external interrupt, usually initiated by the user
    //   SIGSEGV  invalid memory access (segmentation fault)
    //   SIGTERM  termination request, sent to the program
    std::signal(SIGABRT, handleSignal);
    std::signal(SIGFPE, handleSignal);
    std::signal(SIGILL, handleSignal);
    std::signal(SIGINT, handleSignal);
    std::signal(SIGSEGV, handleSignal);
    std::signal(SIGTERM, handleSignal);

    // Fire up main method
    int rc = genie_main(argc, argv);
    if (rc != 0) {
        std::cerr << "Failed to run" << std::endl;
    }

    // The C standard makes no guarantees as to when output to stdout or stderr (standard error) is actually flushed.
    // If e.g. stdout is directed to a file and an error occurs while flushing the data (after program termination),
    // then the output may be lost. Thus we explicitly flush stdout and stderr. On failure, we notify the operating
    // system by returning with EXIT_FAILURE.
    if (fflush(stdout) == EOF) {
        return EXIT_FAILURE;
    }
    if (fflush(stderr) == EOF) {
        return EXIT_FAILURE;
    }

    // Return to the operating system
    return (rc == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
