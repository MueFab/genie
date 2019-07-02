#include "gabacify/analyze.h"

#include <gabac/gabac.h>

#include <fstream>
#include <iostream>

namespace gabacify {
void analyze(const std::string& inputFilePath,
             const std::string& configurationFilePath,
             size_t blocksize,
             uint64_t max_val,
             uint8_t word_size
){
    std::ifstream inputFile;
    std::ofstream configurationFile;
    gabac::NullStream nullstream;

    std::istream *istream = &std::cin;
    std::ostream *ostream = &std::cout;
    std::ostream *logstream = &std::cout;

    if (!inputFilePath.empty()) {
        inputFile = std::ifstream(inputFilePath, std::ios::binary);
        if (!inputFile) {
            GABAC_DIE("Could not open input file");
        }
        istream = &inputFile;
    }
    if (!configurationFilePath.empty()) {
        configurationFile = std::ofstream(configurationFilePath, std::ios::binary);
        if (!configurationFile) {
            GABAC_DIE("Could not open output file");
        }
        ostream = &configurationFile;
    } else {
        logstream = &nullstream;
    }

    gabac::IOConfiguration ioConf = {istream,
                                     ostream,
                                     blocksize,
                                     logstream,
                                     gabac::IOConfiguration::LogLevel::TRACE
    };


    auto aconf = gabac::getCandidateConfig();
    aconf.wordSize = word_size;
    aconf.maxValue = max_val;
    gabac::analyze(ioConf, aconf);
    /* GABACIFY_LOG_INFO << "Wrote smallest bytestream of size "
                      << bestByteStream.size()
                      << " to: "
                      << outputFilePath;*/

    // Write the best configuration as JSON
    /* GABACIFY_LOG_DEBUG << "with configuration: \n"
                       << bestConfig.toPrintableString();
    // GABACIFY_LOG_INFO << "Wrote best configuration to: " << configurationFilePath;*/
}
}  // namespace gabacify
