/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FEATURE_FEATURE_PARSER_H_
#define SRC_GENIE_FEATURE_FEATURE_PARSER_H_

// ---------------------------------------------------------------------------------------------------------------------
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/variantsite/attributes.h"
#include "genie/variantsite/descriptors.h"

#include "genie/annotation/json_attribute_parser.h"
#include "genie/core/parameter/annotation/attribute_data.h"
#include "genie/core/parameter/annotation/attribute_parameter_set.h"
#include "genie/core/parameter/annotation/descriptor_configuration.h"
#include "genie/core/feature_record/record.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace feature {

//-------------------------------------------------------------------------------//

class FeatureParser {
 public:
    using AttributeData = core::parameter::annotation::AttributeData;
    using InfoField = annotation::InfoField;

    FeatureParser(std::istream& _feature_MGrecs, std::vector<annotation::InfoField>& _fields, uint64_t _rowsPerTile);

    size_t getNumberOfRows() const { return numberOfRows; }

    variant_site::Attributes& getAttributes() { return attributes; }

    uint64_t getNrOfTiles() { return attributes.getTiles().begin()->second.getNrOfTiles(); }

 private:
    core::record::feature::Record feature;
    std::istream& featureMGrecs;
    uint64_t rowsPerTile;
    size_t numberOfRows;
    std::map<std::string, core::record::feature::Info_tag> tags;
    std::vector<InfoField> infoFields;
    std::vector<std::string> testAltern;
    std::map<std::string, uint8_t> infoFieldType;

    std::vector<util::BitWriter> fieldWriter;

    std::map<std::string, util::BitWriter> attrWriter;

    std::map<std::string, AttributeData> attributeData;
    variant_site::Attributes attributes;

    uint16_t numberOfAttributes;

    const uint8_t alternEndLine = 0x06;
    const uint8_t alternEnd = 0x07;
    uint64_t startPos = 0;

    void init();
    bool fillRecord(util::BitReader reader);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace feature
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FEATURE_FEATURE_PARSER_H_

// ---------------------------------------------------------------------------------------------------------------------
