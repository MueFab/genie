/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_VARIANT_SITE_VARIANTSITEPARSER_H_
#define SRC_GENIE_CORE_RECORD_VARIANT_SITE_VARIANTSITEPARSER_H_

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
#include "genie/core/record/variant_site/VariantSiteParser.h"
#include "genie/core/record/variant_site/record.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace variant_site {

class VaritanSiteParser {
 public:
    using AttributeData = genie::core::record::variant_site::AttributeData;

    VaritanSiteParser(std::istream& site_MGrecs,
                      std::map<genie::core::record::annotation_parameter_set::DescriptorID, std::stringstream>& output,
                      std::map<std::string, AttributeData>& info,
                      std::map<std::string, std::stringstream>& attributeStream, std::stringstream& jsonInfoFields)
        : siteMGrecs(site_MGrecs),
          dataFields(output),
          variantSite{},
          rowsPerTile(0),
          fieldWriter{},
          numberOfRows(0),
          attributeData(info),
          attributeStream(attributeStream),
          numberOfAttributes(0) {
        infoFieldsJSON = jsonInfoFields.str();
        if (!infoFieldsJSON.empty()) ParseInfoFields();
        init();
        util::BitReader reader(siteMGrecs);
        while (fillRecord(reader)) {
            ParseOne();
            numberOfRows++;
        }
        writeDanglingBits();
    }

    size_t getNumberOfRows() const { return numberOfRows; }

 private:
    using DescriptorID = genie::core::record::annotation_parameter_set::DescriptorID;

    variant_site::Record variantSite;
    std::istream& siteMGrecs;
    uint64_t rowsPerTile;
    size_t numberOfRows;

    std::string infoFieldsJSON;
    std::vector<std::string> infoFields;
    std::map<std::string, uint8_t> infoFieldType;

    std::vector<Writer> fieldWriter;
    std::map<DescriptorID, std::stringstream>& dataFields;

    std::vector<Writer> attributeWriter;
    std::map<std::string, std::stringstream>& attributeStream;
    std::map<std::string, AttributeData> attributeData;
    uint16_t numberOfAttributes;

    const uint8_t AlternEndLine = 0x06;
    const uint8_t AlternEnd = 0x07;

    void addWriter(DescriptorID id);
    void init();
    bool fillRecord(util::BitReader reader);
    void ParseOne();
    void ParseAttribute(uint8_t index);
    void ParseAttribute(std::string infoTagfield);
    void ParseInfoFields();
    void ParseInfoField(std::string id);
    void writeDanglingBits();
    uint8_t AlternTranslate(char alt) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace variant_site
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_VARIANT_SITE_VARIANTSITEPARSER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
