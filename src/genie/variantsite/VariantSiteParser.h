/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_VARIANTSITE_VARIANTSITEPARSER_H_
#define SRC_GENIE_VARIANTSITE_VARIANTSITEPARSER_H_

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
#include "genie/variantsite/Attributes.h"
#include "genie/variantsite/Descriptors.h"

#include "genie/core/record/annotation_parameter_set/AttributeData.h"
#include "genie/core/record/annotation_parameter_set/AttributeParameterSet.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/core/record/variant_site/record.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
#include "genie/annotation/JsonAttributeParser.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {

//-------------------------------------------------------------------------------//

class VariantSiteParser {
 public:
    using AttributeData = genie::core::record::annotation_parameter_set::AttributeData;
    using InfoField = genie::annotation::InfoField;

    VariantSiteParser(std::istream& _site_MGrecs, std::stringstream& _jsonInfoFields, uint64_t rowsPerTile);
    VariantSiteParser(std::istream& _site_MGrecs, std::vector<genie::annotation::InfoField>& _fields, uint64_t _rowsPerTile);

    size_t getNumberOfRows() const { return numberOfRows; }

    Attributes& getAttributes() { return attributes; }
    Descriptors& getDescriptors() { return descriptors; }

    uint64_t getNrOfTiles() { return descriptors.getTiles()[genie::core::AnnotDesc::STARTPOS].getTiles().size(); }

 private:
    genie::core::record::variant_site::Record variantSite;
    std::istream& siteMGrecs;
    uint64_t rowsPerTile;
    size_t numberOfRows;
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

#endif  // SRC_GENIE_VARIANTSITE_VARIANTSITEPARSER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
