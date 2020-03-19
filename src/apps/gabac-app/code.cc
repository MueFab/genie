#include "code.h"

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

#include <genie/entropy/gabac/gabac.h>

namespace gabacify {

//------------------------------------------------------------------------------

void code(const std::string &inputFilePath,
          //RESTRUCT_DISABLE const std::string &configurationFilePath,
          const std::string &outputFilePath,
          size_t blocksize,
          uint8_t descID,
          uint8_t subseqID,
          bool decode) {
    std::ifstream inputFile;
    std::ofstream outputFile;
    genie::entropy::gabac::NullStream nullstream;

    std::istream *istream = &std::cin;
    std::ostream *ostream = &std::cout;
    std::ostream *logstream = &std::cout;

    if (!inputFilePath.empty()) {
        // Read in the entire input file
        inputFile = std::ifstream(inputFilePath, std::ios::binary);
        if (!inputFile) {
            GABAC_DIE("Could not open input file");
        }
        istream = &inputFile;
    }

    if (!outputFilePath.empty()) {
        // Write the bytestream
        outputFile = std::ofstream(outputFilePath, std::ios::binary);
        if (!outputFile) {
            GABAC_DIE("Could not open output file");
        }
        ostream = &outputFile;
    } else {
        logstream = &nullstream;
    }

    genie::entropy::gabac::IOConfiguration ioconf = {istream, ostream, blocksize, logstream,
                                                     genie::entropy::gabac::IOConfiguration::LogLevel::INFO};

    // input string to the internal GABAC configuration
    genie::entropy::gabac::EncodingConfiguration configuration;
    {
        // TODO populate config based on descID, subseqID
    }

    genie::entropy::gabac::run(ioconf, configuration, decode);

    /* GABACIFY_LOG_INFO << "Wrote buffer of size "
                      << outStream.bytesWritten()
                      << " to: "
                      << outputFilePath;*/
}

//------------------------------------------------------------------------------

}  // namespace gabacify

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
