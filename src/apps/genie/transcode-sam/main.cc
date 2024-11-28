/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/transcode-sam/main.h"
#include <format/sam/sam_to_mgrec/transcoder.h>

#include <apps/genie/transcode-sam/program-options.h>

#include <iostream>
#include "genie/format/sam/importer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp::transcode_sam {

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    try {
        genieapp::transcode_sam::Config programOptions(argc, argv);
        if (programOptions.help) {
            return 0;
        }
        if ((programOptions.inputFile.substr(programOptions.inputFile.length() - 3) == "sam" ||
             programOptions.inputFile.substr(programOptions.inputFile.length() - 3) == "bam") &&
            programOptions.outputFile.substr(programOptions.outputFile.length() - 5) == "mgrec") {
            genie::format::sam::sam_to_mgrec::transcode_sam2mpg(programOptions);
        } else if (programOptions.outputFile.substr(programOptions.outputFile.length() - 3) == "sam" &&
                   programOptions.inputFile.substr(programOptions.inputFile.length() - 5) == "mgrec") {
            genie::format::sam::sam_to_mgrec::transcode_mpg2sam(programOptions);
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

}  // namespace genieapp::transcode_sam

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
