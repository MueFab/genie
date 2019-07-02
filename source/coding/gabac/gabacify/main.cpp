#include <cassert>
#include <csignal>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <gabac/gabac.h>

#include "gabacify/analyze.h"
#include "gabacify/code.h"
#include "gabacify/program_options.h"

int main(
        int argc,
        char *argv[]
){
    try {
        gabacify::ProgramOptions programOptions(argc, argv);

        if (programOptions.task == "encode") {
            gabacify::code(
                    programOptions.inputFilePath,
                    programOptions.configurationFilePath,
                    programOptions.outputFilePath,
                    programOptions.blocksize,
                    false
            );
        } else if (programOptions.task == "decode") {
            gabacify::code(
                    programOptions.inputFilePath,
                    programOptions.configurationFilePath,
                    programOptions.outputFilePath,
                    programOptions.blocksize,
                    true
            );
        } else if (programOptions.task == "analyze") {
            gabacify::analyze(
                    programOptions.inputFilePath,
                    programOptions.outputFilePath,
                    programOptions.blocksize,
                    programOptions.maxVal,
                    programOptions.wordSize
            );
        } else {
            GABAC_DIE("Invalid task: " + std::string(programOptions.task));
        }
    }
    catch (const gabac::RuntimeException& e) {
        std::cerr << e.message() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "Unkown error occurred" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
