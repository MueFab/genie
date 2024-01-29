/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_COMPRESSORS_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_COMPRESSORS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/runtime-exception.h"

#include "CompressorParameterSet.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

// ---------------------------------------------------------------------------------------------------------------------
class Compressor {
 public:
    Compressor(uint8_t compressorID);

    uint8_t getNrOfCompressorIDs() const { return static_cast<uint8_t>(compressorParameters.size()); };

    void compress(std::stringstream& input, std::stringstream& output, uint8_t compressorID);

 private:
    uint8_t selectedCompressorID;
    std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet> compressorParameters;

    void initCompressors();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_COMPRESSORS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
