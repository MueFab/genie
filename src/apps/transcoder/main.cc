#include <iostream>

#include "program-options.h"
#include "sam_sorter.h"
#include "sam_transcoder.h"

int main(int argc, char* argv[]) {
    try {
        transcoder::ProgramOptions programOptions(argc, argv);

        uint8_t ret;
        if ((ret = sam_transcoder::sam_to_mgrec(programOptions)) != 0){
            UTILS_DIE("ERROR");
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unkown error occurred" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
