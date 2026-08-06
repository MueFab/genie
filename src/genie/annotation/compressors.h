/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ANNOTATION_COMPRESSORS_H_
#define SRC_GENIE_ANNOTATION_COMPRESSORS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <sstream>
#include <string>
#include <vector>

#include "genie/core/parameter/annotation/compressor_parameter_set.h"
#include "genie/core/access_unit/annotation/typed_data.h"

#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::annotation {

// ---------------------------------------------------------------------------------------------------------------------
class Compressor {
 public:
    Compressor();
    void parseConfig(std::stringstream& config);
    uint8_t getNrOfCompressorIDs() const { return static_cast<uint8_t>(compressorParameters.size()); }

    void compress(std::stringstream& input, std::stringstream& output, uint8_t compressorID);

    const std::vector<genie::core::parameter::annotation::CompressorParameterSet>& getCompressorParameters()
        const {
        return compressorParameters;
    }

 private:
    uint8_t selectedCompressorID;
    std::vector<genie::core::parameter::annotation::CompressorParameterSet> compressorParameters;

    void parseCompressor(std::vector<std::string> commandline);

    genie::entropy::lzma::LZMAParameters readLzmaParameters(std::vector<std::string>& stringpars);
    genie::entropy::bsc::BSCParameters readBscParameters(std::vector<std::string>& stringpars);
    genie::entropy::zstd::ZSTDParameters readZstdParameters(std::vector<std::string>& stringpars);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::annotation

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_COMPRESSORS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
