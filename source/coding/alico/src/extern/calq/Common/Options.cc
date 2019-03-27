/** @file Options.cc
 *  @brief This file contains the implementation of the Options struct.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#include "Common/Options.h"
#include "Common/Exceptions.h"
#include "Common/log.h"

namespace calq {

Options::Options(void)
      // Options for both compression and decompression
    : force(false),
      inputFileName(""),
      outputFileName(""),
      // Options for only compression
      blockSize(0),
      polyploidy(0),
      qualityValueMax(0),
      qualityValueMin(0),
      qualityValueOffset(0),
      qualityValueType(""),
      referenceFileNames(),
      // Options for only decompression
      decompress(false),
      sideInformationFileName("") {}

Options::~Options(void) {}

void Options::validate(void) {
    // force
    if (force == true) {
        CALQ_LOG("Force switch set - overwriting output file(s)");
    }

    // inputFileName
    CALQ_LOG("Input file name: %s", inputFileName.c_str());
    if (inputFileName.empty() == true) {
        throwErrorException("No input file name provided");
    }
    if (decompress == false) {
        if (fileNameExtension(inputFileName) != std::string("sam")) {
            throwErrorException("Input file name extension must be 'sam'");
        }
    } else {
        if (fileNameExtension(inputFileName) != std::string("cq")) {
            CALQ_LOG("Warning: Input file name extension is not 'cq'");
//             throwErrorException("Input file name extension must be 'cq'");
        }
    }
    if (fileExists(inputFileName) == false) {
        throwErrorException("Cannot access input file");
    }

    // outputFileName
    if (decompress == false) {
        if (outputFileName.empty() == true) {
            CALQ_LOG("No output file name provided - constructing output file name from input file name");
            outputFileName += inputFileName + ".cq";
        }
    } else {
        if (outputFileName.empty() == true) {
            CALQ_LOG("No output file name provided - constructing output file name from input file name");
            outputFileName += inputFileName + ".qual";
        }
    }
    CALQ_LOG("Output file name: %s", outputFileName.c_str());
    if (fileExists(outputFileName) == true) {
        if (force == false) {
            throwErrorException("Not overwriting output file (use option 'f' to force overwriting)");
        }
    }

    // blockSize
    if (decompress == false) {
        CALQ_LOG("Block size: %d", blockSize);
        if (blockSize < 1) {
            throwErrorException("Block size must be greater than 0");
        }
    }

    // polyploidy
    if (decompress == false) {
        CALQ_LOG("Polyploidy: %d", polyploidy);
        if (polyploidy < 1) {
            throwErrorException("Polyploidy must be greater than 0");
        }
    }

    // qualityValueType
    if (decompress == false) {
        CALQ_LOG("Quality value type: %s", qualityValueType.c_str());
        if (qualityValueType == "Sanger") {
            // Sanger: Phred+33 [0,40]
            qualityValueOffset = 33;
            qualityValueMin = 0;
            qualityValueMax = 40;
        } else if (qualityValueType == "Illumina-1.3+") {
            // Illumina 1.3+: Phred+64 [0,40]
            qualityValueOffset = 64;
            qualityValueMin = 0;
            qualityValueMax = 40;
        } else if (qualityValueType == "Illumina-1.5+") {
            // Illumina 1.5+: Phred+64 [0,40] with 0=unused, 1=unused, 2=Read Segment Quality Control Indicator ('B')
            CALQ_LOG("Warning: Read Segment Quality Control Indicator will not be treated specifically by CALQ");
            qualityValueOffset = 64;
            qualityValueMin = 0;
            qualityValueMax = 40;
        } else if (qualityValueType == "Illumina-1.8+") {
            // Illumina 1.8+ Phred+33 [0,41]
            qualityValueOffset = 33;
            qualityValueMin = 0;
            qualityValueMax = 41;
        } else if (qualityValueType == "Max33") {
            // Max33 Phred+33 [0,93]
            qualityValueOffset = 33;
            qualityValueMin = 0;
            qualityValueMax = 93;
        } else if (qualityValueType == "Max64") {
            // Max64 Phred+64 [0,62]
            qualityValueOffset = 64;
            qualityValueMin = 0;
            qualityValueMax = 62;
        } else {
            throwErrorException("Quality value type not supported");
        }
        CALQ_LOG("Quality value offset: %d", qualityValueOffset);
        CALQ_LOG("Quality value range: [%d,%d]", qualityValueMin, qualityValueMax);

        // referenceFiles
        if (decompress == false) {
            if (referenceFileNames.empty() == true) {
                CALQ_LOG("Operating without reference file(s)");
            } else {
                CALQ_LOG("Operating with %zu reference file(s):", referenceFileNames.size());
                for (auto const &referenceFileName : referenceFileNames) {
                    CALQ_LOG("  %s", referenceFileName.c_str());
                    if (referenceFileName.empty() == true) {
                        throwErrorException("Reference file name not proviced");
                    }
                    if (fileExists(referenceFileName) == false) {
                        throwErrorException("Cannot access reference file");
                    }
                    if (fileNameExtension(referenceFileName) != std::string("fa")
                        && fileNameExtension(referenceFileName) != std::string("fasta")) {
                        throwErrorException("Reference file name extension must be 'fa' or 'fasta'");
                    }
                }
            }
        }
    }

    // decompress
    if (decompress == false) {
        CALQ_LOG("Compressing");
    } else {
        CALQ_LOG("Decompressing");
    }

    // sideInformationFileName
    if (decompress == true) {
        CALQ_LOG("Side information file name: %s", sideInformationFileName.c_str());
        if (sideInformationFileName.empty() == true) {
            throwErrorException("No side information file name provided");
        }
        if (fileNameExtension(sideInformationFileName) != std::string("sam")) {
            throwErrorException("Side information file name extension must be 'sam'");
        }
        if (fileExists(sideInformationFileName) == false) {
            throwErrorException("Cannot access side information file");
        }
    }
}

}  // namespace calq

