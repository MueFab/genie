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

class AttributeData {
 public:
    AttributeData() : attributeNameLength(0), attributeName(""), attributeType(0), attributeArrayDims(0), value{} {}
    AttributeData(uint8_t length, std::string name, uint8_t type, uint8_t arrayLength)
        : attributeNameLength(length),
          attributeName(name),
          attributeType(type),
          attributeArrayDims(arrayLength),
          value{} {}

    AttributeData& operator=(const AttributeData& other) {
        attributeNameLength = other.attributeNameLength;
        attributeName = other.attributeName;
        attributeType = other.attributeType;
        attributeArrayDims = other.attributeArrayDims;
        value << other.value.rdbuf();
        return *this;
    }

    AttributeData(const AttributeData& other) {
        attributeNameLength = other.attributeNameLength;
        attributeName = other.attributeName;
        attributeType = other.attributeType;
        attributeArrayDims = other.attributeArrayDims;
        value << other.value.rdbuf();
    }

    uint8_t getAttributeNameLength() const { return attributeNameLength; }
    std::string getAttributeName() const { return attributeName; }
    uint8_t getAttributeType() const { return attributeType; }
    uint8_t getArrayLength() const { return attributeArrayDims; }
    std::stringstream& getValue() { return value; }

 private:
    uint8_t attributeNameLength;
    std::string attributeName;
    uint8_t attributeType;
    uint8_t attributeArrayDims;
    std::stringstream value;
};

class VaritanSiteParser {
 public:
    VaritanSiteParser(std::istream& site_MGrecs,
                      std::map<genie::core::record::annotation_parameter_set::DescriptorID, std::stringstream>& output)
        : siteMGrecs(site_MGrecs),
          dataFields(output),
          variantSite{},
          rowsPerTile(0),
          fieldWriter{},
          numberOfRows(0),
          attributeInfo{} {
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
    std::map<DescriptorID, std::stringstream>& dataFields;
    std::vector<Writer> fieldWriter;
    std::vector<AttributeData> attributeInfo;
    size_t numberOfRows;

    void addWriter(DescriptorID id);
    void init();
    bool fillRecord(util::BitReader reader);
    void ParseOne();
    void ParseAttribute();
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
