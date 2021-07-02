/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/transcode-sam/main.h"
#include <iostream>
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
        genieapp::transcode_sam::ErrorCode ret = genieapp::transcode_sam::ErrorCode::success;
        if (programOptions.inputFile.substr(programOptions.inputFile.length() - 3) == "sam" &&
            programOptions.outputFile.substr(programOptions.outputFile.length() - 5) == "mgrec") {
            ret = transcode_sam2mpg(programOptions);
        } else if (programOptions.outputFile.substr(programOptions.outputFile.length() - 3) == "sam" &&
                   programOptions.inputFile.substr(programOptions.inputFile.length() - 5) == "mgrec") {
            ret = transcode_mpg2sam(programOptions);
        }
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
