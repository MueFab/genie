/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_VARIANTSITE_VARIANTSITE_PARSER_H_
#define SRC_GENIE_VARIANTSITE_VARIANTSITE_PARSER_H_

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
#include "genie/annotation/attributes.h"
#include "genie/annotation/descriptors.h"

#include "genie/annotation/json_attribute_parser.h"
#include "genie/core/parameter/annotation/attribute_data.h"
#include "genie/core/parameter/annotation/attribute_parameter_set.h"
#include "genie/core/parameter/annotation/descriptor_configuration.h"
#include "genie/core/record/site/record.h"
#include "genie/core/writer.h"
#include "genie/util/bit_reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {

//-------------------------------------------------------------------------------//

class VariantSiteParser {
 public:
    using AttributeData = genie::core::parameter::annotation::AttributeData;
    using InfoField = genie::annotation::InfoField;

    VariantSiteParser(std::istream& _site_MGrecs, std::stringstream& _jsonInfoFields, uint64_t rowsPerTile);
    VariantSiteParser(std::istream& _site_MGrecs, std::vector<genie::annotation::InfoField>& _fields, uint64_t _rowsPerTile);
    VariantSiteParser(std::istream& _site_MGrecs, std::map<std::string, genie::core::record::variant_site::Info_tag>& _fields, uint64_t _rowsPerTile);

    size_t getNumberOfRows() const { return numberOfRows; }

    Attributes& getAttributes() { return attributes; }
    Descriptors& getDescriptors() { return descriptors; }

    uint64_t getNrOfTiles() { return descriptors.getTiles()[genie::core::AnnotDesc::STARTPOS].getTiles().size(); }

 private:
    genie::core::record::variant_site::Record variantSite;
    std::istream& siteMGrecs;
    uint64_t rowsPerTile;
    size_t numberOfRows;
    std::map<std::string, genie::core::record::variant_site::Info_tag> tags;
    std::vector<InfoField> infoFields;
    std::vector<std::string> testAltern;
    std::map<std::string, uint8_t> infoFieldType;

    std::vector<genie::core::Writer> fieldWriter;

    std::map<std::string, genie::core::Writer> attrWriter;

    std::map<std::string, AttributeData> attributeData;
    Attributes attributes;
    Descriptors descriptors;

    uint16_t numberOfAttributes;

    const uint8_t alternEndLine = 0x06;
    const uint8_t alternEnd = 0x07;
    uint64_t startPos = 0;

    void init();
    bool fillRecord(util::BitReader reader);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace variant_site
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_VARIANTSITE_VARIANTSITE_PARSER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
