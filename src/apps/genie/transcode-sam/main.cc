/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <iostream>

// ---------------------------------------------------------------------------------------------------------------------

#include "apps/genie/transcode-sam/sam/sam_to_mgrec/program-options.h"
#include "apps/genie/transcode-sam/sam/sam_to_mgrec/sorter.h"
#include "apps/genie/transcode-sam/sam/sam_to_mgrec/transcoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace transcode_sam {

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    try {
        genieapp::transcode_sam::sam::sam_to_mgrec::Config programOptions(argc, argv);

        genieapp::transcode_sam::ErrorCode ret = transcode(programOptions);
        if (ret != genieapp::transcode_sam::ErrorCode::success) {
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

}  // namespace transcode_sam
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
