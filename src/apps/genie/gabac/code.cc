/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/gabac/code.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/entropy/gabac/gabac.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

void code(const std::string &inputFilePath, const std::string &outputFilePath, size_t blocksize, uint8_t descID,
          uint8_t subseqID, bool decode, const std::string &dependencyFilePath) {
    std::ifstream inputFile;
    std::ifstream dependencyFile;
    std::ofstream outputFile;
    genie::entropy::gabac::NullStream nullstream;

    std::istream *istream = &std::cin;
    std::istream *dstream = nullptr;
    std::ostream *ostream = &std::cerr;
    std::ostream *logstream = &std::cerr;

    if (!inputFilePath.empty()) {
        // Read in the entire input file
        inputFile = std::ifstream(inputFilePath, std::ios::binary);
        if (!inputFile) {
            UTILS_DIE("Could not open input file");
        }
        istream = &inputFile;
    }

    if (!dependencyFilePath.empty()) {
        // Read in the entire dependency file
        dependencyFile = std::ifstream(dependencyFilePath, std::ios::binary);
        if (!inputFile) {
            UTILS_DIE("Could not open dependency file");
        }
        dstream = &dependencyFile;
    }

    if (!outputFilePath.empty()) {
        // Write the bytestream
        outputFile = std::ofstream(outputFilePath, std::ios::binary);
        if (!outputFile) {
            UTILS_DIE("Could not open output file");
        }
        ostream = &outputFile;
    } else {
        logstream = &nullstream;
    }

    genie::entropy::gabac::IOConfiguration ioconf = {
        istream, dstream, ostream, blocksize, logstream, genie::entropy::gabac::IOConfiguration::LogLevel::LOG_INFO};

    genie::core::GenSubIndex genieSubseqID =
        (genie::core::GenSubIndex)std::pair<genie::core::GenDesc, uint8_t>((genie::core::GenDesc)descID, subseqID);

    genie::entropy::gabac::run(
        ioconf,
        genie::entropy::gabac::EncodingConfiguration(genie::entropy::gabac::getEncoderConfigManual(genieSubseqID)),
        decode);

    /* GABACIFY_LOG_INFO << "Wrote buffer of size "
                      << outStream.bytesWritten()
                      << " to: "
                      << outputFilePath;*/
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
