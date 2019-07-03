#include <iostream>

// #include "lae/encode.h"
// #include "lae/program_options.h"

int main(int argc, char *argv[]) {
    try {
        // lae::ProgramOptions programOptions(argc, argv);
        // lae::encode(programOptions.inputFilePath, programOptions.outputFilePath);
    }
    // catch (const lae::RuntimeException& e) {
    //     std::cerr << e.message() << std::endl;
    //     return EXIT_FAILURE;
    // }
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
