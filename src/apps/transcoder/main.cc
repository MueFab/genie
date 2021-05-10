#include <iostream>

#include "program-options.h"
#include "sam_transcoder.h"

int main(int argc, char* argv[]) {
    try {
        transcoder::ProgramOptions programOptions(argc, argv);

        sam_transcoder::sam_to_mgrec(programOptions);

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unkown error occurred" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
