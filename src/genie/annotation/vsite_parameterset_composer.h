/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
 
#ifndef SRC_GENIE_ANNOTATION_VSITE_PARAMETERSET_COMPOSER_H_
#define SRC_GENIE_ANNOTATION_VSITE_PARAMETERSET_COMPOSER_H_
 
// ---------------------------------------------------------------------------------------------------------------------
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
 
#include "genie/core/array_type.h"
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bit_reader.h"
 
#include "genie/core/parameter/annotation/record.h"
#include "genie/core/parameter/annotation/attribute_data.h"
#include "genie/annotation/compressors.h"
// ---------------------------------------------------------------------------------------------------------------------
 
namespace genie {
namespace variant_site {
 
class ParameterSetComposer {
 public:
    ParameterSetComposer() {}
 
    genie::core::parameter::annotation::Record setParameterSet(
        std::vector<genie::core::AnnotDesc> descrList,
        std::map<std::string, genie::core::parameter::annotation::AttributeData>& info,
        uint64_t defaultTileSize, uint8_t AT_ID);
 
    genie::core::parameter::annotation::Record setParameterSet(
        std::vector<genie::core::AnnotDesc> descrList,
        std::map<std::string, genie::core::parameter::annotation::AttributeData>& info,
        const std::vector<genie::core::parameter::annotation::CompressorParameterSet>& compressors,
        uint64_t defaultTileSize, uint8_t _AT_ID);
 
    genie::core::parameter::annotation::Record setParameterSet(
        std::vector<genie::core::AnnotDesc> descrList,
        std::map<std::string, genie::core::parameter::annotation::AttributeData>& info,
        genie::annotation::Compressor compressors,
        uint64_t defaultTileSize);
 
 private:
};
 
// ---------------------------------------------------------------------------------------------------------------------
 
}  // namespace variant_site
}  // namespace genie
 
// ---------------------------------------------------------------------------------------------------------------------
 
#endif  // SRC_GENIE_ANNOTATION_VSITE_PARAMETERSET_COMPOSER_H_
