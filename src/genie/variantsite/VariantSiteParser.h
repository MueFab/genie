/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_VARIANT_SITE_VARIANTSITEPARSER_H_
#define SRC_GENIE_VARIANT_SITE_VARIANTSITEPARSER_H_

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

#include "genie/core/record/annotation_parameter_set/AttributeParameterSet.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/variantsite/JsonInfoFieldParser.h"
#include "genie/core/record/variant_site/record.h"
#include "genie/core/record/annotation_parameter_set/AttributeData.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {


class VariantSiteParser {
 public:
    using AttributeData = genie::core::record::annotation_parameter_set::AttributeData;
    using InfoField = genie::variant_site::InfoField;

    VariantSiteParser(std::istream& _site_MGrecs,
                      std::map<genie::core::AnnotDesc, std::stringstream>& _output,
                      std::map<std::string, AttributeData>& _info,
                      std::map<std::string, std::stringstream>& _attributeStream,
                      std::stringstream& _jsonInfoFields);

    size_t getNumberOfRows() const { return numberOfRows; }

 private:
//    using DescriptorID = genie::core::record::annotation_parameter_set::DescriptorID;

    genie::core::record::variant_site::Record variantSite;
    std::istream& siteMGrecs;
    uint64_t rowsPerTile;
    size_t numberOfRows;
    std::vector<std::string> testAltern;
    std::vector<InfoField> infoFields;
    std::map<std::string, uint8_t> infoFieldType;

    std::vector<genie::core::Writer> fieldWriter;
    std::map<genie::core::AnnotDesc, std::stringstream>& dataFields;

    std::map<std::string, genie::core::Writer> attrWriter;
    std::map<std::string, std::stringstream>& attributeStream;
    std::map<std::string, AttributeData>& attributeData;
    uint16_t numberOfAttributes;

    const uint8_t alternEndLine = 0x06;
    const uint8_t alternEnd = 0x07;
    uint64_t startPos = 0;

    void addWriter(genie::core::AnnotDesc id);
    void init();
    bool fillRecord(util::BitReader reader);
    void ParseOne();
    void ParseAttribute(uint8_t index);
    void ParseAttribute(const std::string& infoTagfield);
    void writeDanglingBits();
    uint8_t AlternTranslate(char alt) const;
    static std::vector<uint8_t> FilterTranslate(const std::string& filter) ;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace variant_site
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_VARIANT_SITE_VARIANTSITEPARSER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
