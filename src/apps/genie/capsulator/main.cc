/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/capsulator/main.h"
#include <fstream>
#include <iostream>
#include "apps/genie/capsulator/program-options.h"
#include "genie/format/mgb/raw_reference.h"
#include "genie/format/mpegg_p1/mpegg_file.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace capsulator {

// ---------------------------------------------------------------------------------------------------------------------

enum class ErrorCode : uint8_t { success = 0, failure = 1 };
enum class DataUnitType : uint8_t { RAW_REF = 0, PAR_SET = 1, ACC_UNT = 2 };

// ---------------------------------------------------------------------------------------------------------------------

ErrorCode encapsulate(ProgramOptions&) { return ErrorCode::success; }

// ---------------------------------------------------------------------------------------------------------------------

ErrorCode decapsulate(ProgramOptions& options) {
    std::ifstream reader(options.inputFile);
    genie::util::BitReader bitreader(reader);

    genie::format::mpegg_p1::MpeggFile mpegg_file(bitreader);

    return ErrorCode::success;
}

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    ProgramOptions pOpts(argc, argv);

    genieapp::capsulator::ErrorCode ret = genieapp::capsulator::ErrorCode::success;
    /// Encapsulate
    if (pOpts.inputFile.substr(pOpts.inputFile.length() - 3) == "mgb" &&
        pOpts.outputFile.substr(pOpts.outputFile.length() - 3) == "mgg") {
        ret = encapsulate(pOpts);
        /// Decapsulate
    } else if (pOpts.inputFile.substr(pOpts.inputFile.length() - 3) == "mgg" &&
               pOpts.outputFile.substr(pOpts.outputFile.length() - 3) == "mgb") {
        ret = decapsulate(pOpts);
    }
    if (ret != genieapp::capsulator::ErrorCode::success) {
        UTILS_DIE("ERROR");
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace capsulator
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
