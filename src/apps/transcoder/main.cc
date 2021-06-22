/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <iostream>

// ---------------------------------------------------------------------------------------------------------------------

#include "apps/transcoder/sam/sam_to_mgrec/program-options.h"
#include "apps/transcoder/sam/sam_to_mgrec/sorter.h"
#include "apps/transcoder/sam/sam_to_mgrec/transcoder.h"

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    try {
        genie::transcoder::sam::sam_to_mgrec::Config programOptions(argc, argv);

        genie::transcoder::ErrorCode ret = transcode(programOptions);
        if (ret != genie::transcoder::ErrorCode::success) {
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

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
