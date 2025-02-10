/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_VARIANTSITE_PARAMETERSETCOMPOSER_H_
#define SRC_GENIE_VARIANTSITE_PARAMETERSETCOMPOSER_H_

// ---------------------------------------------------------------------------------------------------------------------
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/arrayType.h"
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bit_reader.h"

#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/record/annotation_parameter_set/AttributeData.h"
#include "genie/annotation/compressors.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {

class ParameterSetComposer {
 public:
    ParameterSetComposer() {}

    genie::core::record::annotation_parameter_set::Record setParameterSet(
        std::vector<genie::core::AnnotDesc> descrList,
        std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData>& info,
        uint64_t defaultTileSize, uint8_t AT_ID);

    genie::core::record::annotation_parameter_set::Record setParameterSet(
        std::vector<genie::core::AnnotDesc> descrList,
        std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData>& info,
        const std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet>& compressors,
        uint64_t defaultTileSize, uint8_t _AT_ID);

    genie::core::record::annotation_parameter_set::Record setParameterSet(
        std::vector<genie::core::AnnotDesc> descrList,
        std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData>& info,
        genie::annotation::Compressor compressors,
        uint64_t defaultTileSize);

private:
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace variant_site
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_VARIANTSITE_PARAMETERSETCOMPOSER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
