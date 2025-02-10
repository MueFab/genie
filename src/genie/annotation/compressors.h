/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ANNOTATION_COMPRESSORS_H_
#define SRC_GENIE_ANNOTATION_COMPRESSORS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/core/record/annotation_parameter_set/CompressorParameterSet.h"

#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

// ---------------------------------------------------------------------------------------------------------------------
class Compressor {
 public:
    Compressor();
    void parseConfig(std::stringstream& config);
    uint8_t getNrOfCompressorIDs() const { return static_cast<uint8_t>(compressorParameters.size()); }

    void compress(std::stringstream& input, std::stringstream& output, uint8_t compressorID);

    const std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet>& getCompressorParameters()
        const {
        return compressorParameters;
    }

 private:
    uint8_t selectedCompressorID;
    std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet> compressorParameters;

    void parseCompressor(std::vector<std::string> commandline);

    genie::entropy::lzma::LZMAParameters readLzmaParameters(std::vector<std::string>& stringpars);
    genie::entropy::bsc::BSCParameters readBscParameters(std::vector<std::string>& stringpars);
    genie::entropy::zstd::ZSTDParameters readZstdParameters(std::vector<std::string>& stringpars);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_COMPRESSORS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
