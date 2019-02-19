#include <cassert>
#include <csignal>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "conformance/encoding.h"
#include "conformance/exceptions.h"
#include "conformance/log.h"
#include "conformance/program_options.h"


static void writeCommandLine(int argc, char* argv[])
{
    std::vector<std::string> args(argv, (argv + argc));
    std::stringstream commandLine;
    for (const auto& arg : args) {
        commandLine << arg << " ";
    }
    GENIE_LOG_DEBUG << "Command line: " << commandLine.str();
}


static int genie_main(int argc, char* argv[])
{
    try {
        genie::ProgramOptions programOptions(argc, argv);
        writeCommandLine(argc, argv);
        genie::encode(programOptions);
    } catch (const genie::RuntimeException& e) {
        GENIE_LOG_ERROR << "Runtime error: " << e.message();
        return -1;
    } catch (const std::exception& e) {
        GENIE_LOG_ERROR << "Standard library error: " << e.what();
        return -1;
    } catch (...) {
        GENIE_LOG_ERROR << "Unknown error occurred";
        return -1;
    }

    return 0;
}


extern "C" void handleSignal(int sig)
{
    std::signal(sig, SIG_IGN);  // Ignore the signal
    std::string signalString;
    switch (sig) {
        case SIGINT:
            signalString = "SIGINT";
            break;
        case SIGILL:
            signalString = "SIGILL";
            break;
        case SIGABRT:
            signalString = "SIGABRT";
            break;
        case SIGFPE:
            signalString = "SIGFPE";
            break;
        case SIGSEGV:
            signalString = "SIGSEGV";
            break;
        case SIGTERM:
            signalString = "SIGTERM";
            break;
        case SIGHUP:
            signalString = "SIGHUP";
            break;
        case SIGQUIT:
            signalString = "SIGQUIT";
            break;
        case SIGTRAP:
            signalString = "SIGTRAP";
            break;
        case SIGKILL:
            signalString = "SIGKILL";
            break;
        case SIGBUS:
            signalString = "SIGBUS";
            break;
        case SIGSYS:
            signalString = "SIGSYS";
            break;
        case SIGPIPE:
            signalString = "SIGPIPE";
            break;
        default:
            signalString = "unknown";
            break;
    }
    GENIE_LOG_WARNING << "Caught signal: " << sig << " (" << signalString << ")";
    std::signal(sig, SIG_DFL);  // Invoke the default signal action
    std::raise(sig);
}


int main(int argc, char* argv[])
{
    // Install signal handler for the following signal types:
    //   SIGTERM  termination request, sent to the program
    //   SIGSEGV  invalid memory access (segmentation fault)
    //   SIGINT   external interrupt, usually initiated by the user
    //   SIGILL   invalid program image, such as invalid instruction
    //   SIGABRT  abnormal termination condition, as is e.g. initiated by
    //            std::abort()
    //   SIGFPE   erroneous arithmetic operation such as divide by zero
    std::signal(SIGABRT, handleSignal);
    std::signal(SIGFPE, handleSignal);
    std::signal(SIGILL, handleSignal);
    std::signal(SIGINT, handleSignal);
    std::signal(SIGSEGV, handleSignal);
    std::signal(SIGTERM, handleSignal);

    // Fire up main method
    int rc = genie_main(argc, argv);
    if (rc != 0) {
        GENIE_LOG_FATAL << "Failed to run";
    }

    // The C standard makes no guarantees as to when output to stdout or
    // stderr (standard error) is actually flushed.
    // If e.g. stdout is directed to a file and an error occurs while flushing
    // the data (after program termination), then the output may be lost.
    // Thus we explicitly flush stdout and stderr. On failure, we notify the
    // operating system by returning with EXIT_FAILURE.
    if (fflush(stdout) == EOF) {
        return EXIT_FAILURE;
    }
    if (fflush(stderr) == EOF) {
        return EXIT_FAILURE;
    }

    // Return to the operating system
    return (rc == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
