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
        if (programOptions.help) {
            return 0;
        }
        if ((programOptions.inputFile.substr(programOptions.inputFile.length() - 3) == "sam" ||
             programOptions.inputFile.substr(programOptions.inputFile.length() - 3) == "bam") &&
            programOptions.outputFile.substr(programOptions.outputFile.length() - 5) == "mgrec") {
            transcode_sam2mpg(programOptions);
        } else if (programOptions.outputFile.substr(programOptions.outputFile.length() - 3) == "sam" &&
                   programOptions.inputFile.substr(programOptions.inputFile.length() - 5) == "mgrec") {
            transcode_mpg2sam(programOptions);
        } else {
            UTILS_DIE("Unknown operation. Check the file extensions of inputs and outputs.");
        }
    } catch (const genie::util::Exception& e) {
        std::cerr << "Genie error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::runtime_error& e) {
        std::cerr << "Std error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown error!" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace transcode_sam
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
