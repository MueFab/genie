#include "VariantSiteParser.h"
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>

#include "genie/core/arrayType.h"
#include "genie/core/record/variant_site/VariantSiteParser.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace variant_site {

VariantSiteParser::VariantSiteParser(
                std::istream& site_MGrecs,
                std::map<AnnotDesc, std::stringstream>& output,
                std::map<std::string, AttributeData>& info,
                std::map<std::string, std::stringstream>& attributeStream,
                std::stringstream& jsonInfoFields)
    : siteMGrecs(site_MGrecs),
      rowsPerTile(0),
      numberOfRows(0),
      fieldWriter{},
      dataFields(output),
      attributeStream(attributeStream),
      attributeData(info),
      numberOfAttributes(0),
      startPos(0) {

    JsonInfoFieldParser InfoFieldParser(jsonInfoFields);
    infoFields = InfoFieldParser.getInfoFields();
    init();
    util::BitReader reader(siteMGrecs);
    while (fillRecord(reader)) {
        ParseOne();
        numberOfRows++;
    }
    writeDanglingBits();
}

void VariantSiteParser::addWriter(AnnotDesc id) {
    dataFields[id];
    Writer writer(&dataFields[id]);
    fieldWriter[static_cast<size_t>(id)] = writer;
}

void VariantSiteParser::init() {
    std::stringstream tempStream;
    fieldWriter.resize(static_cast<size_t>(AnnotDesc::FILTER) + 1, Writer(&tempStream));
    addWriter(AnnotDesc::SEQUENCEID);      // 1
    addWriter(AnnotDesc::STARTPOS);        // 2
    addWriter(AnnotDesc::STRAND);          // 4
    addWriter(AnnotDesc::NAME);            // 5
    addWriter(AnnotDesc::DESCRIPTION);     // 6
    addWriter(AnnotDesc::LINKNAME);        // 7
    addWriter(AnnotDesc::LINKID);          // 8
    addWriter(AnnotDesc::DEPTH);           // 9
    addWriter(AnnotDesc::SEQQUALITY);      // 10
    addWriter(AnnotDesc::MAPQUALITY);      // 11
    addWriter(AnnotDesc::MAPNUMQUALITY0);  // 12
    addWriter(AnnotDesc::REFERENCE);       // 13
    addWriter(AnnotDesc::ALTERN);          // 14
    addWriter(AnnotDesc::FILTER);          // 17
                                              // addWriter(DescriptorID::ATTRIBUTE);       // 31
    uint16_t attributeID = 0;
    for (const auto& infoField : infoFields) {
        Writer writer(&attributeStream[infoField.ID]);
        attrWriter[infoField.ID] = writer;
        AttributeData attribute(static_cast<uint8_t>(infoField.ID.length()), infoField.ID, infoField.Type,
                                infoField.Number, attributeID);
        attributeData[infoField.ID] = attribute;
        attributeID++;
    }

    numberOfAttributes = attributeID;
}

bool VariantSiteParser::fillRecord(util::BitReader reader) {
    if (!variantSite.read(reader)) return false;
    return true;
}

void VariantSiteParser::ParseOne() {
    fieldWriter[static_cast<size_t>(AnnotDesc::SEQUENCEID)].write(variantSite.getSeqID(), 16);
    startPos = variantSite.getPos() - startPos;
    fieldWriter[static_cast<size_t>(AnnotDesc::STARTPOS)].write(startPos, 64);
    fieldWriter[static_cast<size_t>(AnnotDesc::STRAND)].write(variantSite.getStrand(), 2);
    fieldWriter[static_cast<size_t>(AnnotDesc::NAME)].write(variantSite.getID());
    fieldWriter[static_cast<size_t>(AnnotDesc::NAME)].write(0, 8);
    fieldWriter[static_cast<size_t>(AnnotDesc::DESCRIPTION)].write(variantSite.getDescription());
    fieldWriter[static_cast<size_t>(AnnotDesc::DESCRIPTION)].write(0, 8);

    const auto& reference = variantSite.getRef();
    for (const auto& ref : reference) {
        fieldWriter[static_cast<size_t>(AnnotDesc::REFERENCE)].write(AlternTranslate(ref), 3);
    }
    fieldWriter[static_cast<size_t>(AnnotDesc::REFERENCE)].write(alternEnd, 3);

    const auto& altArray = variantSite.getAlt();
    for (auto i = 0; i < variantSite.getAltCount(); ++i) {
        const auto& altern = altArray[i];
        for (const auto& alt : altern) {
            fieldWriter[static_cast<size_t>(AnnotDesc::ALTERN)].write(AlternTranslate(alt), 3);
        }
        fieldWriter[static_cast<size_t>(AnnotDesc::ALTERN)].write(alternEndLine, 3);
    }
    fieldWriter[static_cast<size_t>(AnnotDesc::ALTERN)].write(alternEnd, 3);

    fieldWriter[static_cast<size_t>(AnnotDesc::DEPTH)].write(variantSite.getDepth(), 32);
    fieldWriter[static_cast<size_t>(AnnotDesc::SEQQUALITY)].write(variantSite.getSeqQual(), 32);
    fieldWriter[static_cast<size_t>(AnnotDesc::MAPQUALITY)].write(variantSite.getMapQual(), 32);
    fieldWriter[static_cast<size_t>(AnnotDesc::MAPNUMQUALITY0)].write(variantSite.getMapNumQual0(), 32);
    auto filters = FilterTranslate(variantSite.getFilters());
    for (auto filter : filters) fieldWriter[static_cast<size_t>(AnnotDesc::FILTER)].write(filter, 8);
    if (variantSite.isLinkedRecord()) {
        fieldWriter[static_cast<size_t>(AnnotDesc::LINKNAME)].write(variantSite.getLinkName());
        fieldWriter[static_cast<size_t>(AnnotDesc::LINKNAME)].write(0, 8);
        fieldWriter[static_cast<size_t>(AnnotDesc::LINKID)].write(variantSite.getReferenceBoxID(), 8);
    } else {
        fieldWriter[static_cast<size_t>(AnnotDesc::LINKID)].write(255, 8);
    }
    for (auto & it : attributeData) {
        if (it.first == "HOMSEQ") {
            std::cout << it.first;
        }
        ParseAttribute(it.first);
    }
}

void VariantSiteParser::ParseAttribute(uint8_t index) {
    const auto infoTag = variantSite.getInfoTag()[index].info_tag;
    if (attributeData[infoTag].getAttributeType() != variantSite.getInfoTag()[index].info_type) {
        attributeData[infoTag].setAttributeType(variantSite.getInfoTag()[index].info_type);
    }
    if (attributeData[infoTag].getArrayLength() < variantSite.getInfoTag()[index].info_array_len) {
        attributeData[infoTag].setArrayLength(variantSite.getInfoTag()[index].info_array_len);
    }

    const auto& tag = variantSite.getInfoTag();
    for (const auto& value : tag[index].infoValue) {
        arrayType toval{};
        toval.toFile(attributeData[infoTag].getAttributeType(), value, attrWriter[infoTag]); 
 //       if (attributeData[infoTag].getAttributeType() == 0) attrWriter[infoTag].write(0, 8, true);
    }
}

void VariantSiteParser::ParseAttribute(const std::string& infoTagfield) {
    const auto& infoArray = variantSite.getInfoTag();
    auto matchLocation = static_cast<uint8_t>(infoArray.size());
    for (size_t i = 0; i < infoArray.size(); ++i) {
        if (infoArray[i].info_tag == infoTagfield) {
            matchLocation = i;
            break;
        }
    }
    if (matchLocation < infoArray.size()) {
        ParseAttribute(matchLocation);
    } else {  // insert default values
        arrayType def{};
        auto defaultType = attributeData[infoTagfield].getAttributeType();
        auto defaultValue = def.getDefaultValue(defaultType);
        auto defaultSize = def.getDefaultBitsize(defaultType);
        for (auto i = 0; i < attributeData[infoTagfield].getArrayLength(); ++i) {
            attrWriter[infoTagfield].write(defaultValue, defaultSize);
            if (defaultType == 0) attrWriter[infoTagfield].write(0, 8, true);
        }
    }
}

void VariantSiteParser::writeDanglingBits() {
    fieldWriter[static_cast<size_t>(AnnotDesc::SEQUENCEID)].flush();
    fieldWriter[static_cast<size_t>(AnnotDesc::STARTPOS)].flush();
    fieldWriter[static_cast<size_t>(AnnotDesc::STRAND)].flush();
    fieldWriter[static_cast<size_t>(AnnotDesc::NAME)].flush();
    fieldWriter[static_cast<size_t>(AnnotDesc::DESCRIPTION)].flush();
    fieldWriter[static_cast<size_t>(AnnotDesc::REFERENCE)].flush();
    fieldWriter[static_cast<size_t>(AnnotDesc::ALTERN)].flush();
    fieldWriter[static_cast<size_t>(AnnotDesc::DEPTH)].flush();
    fieldWriter[static_cast<size_t>(AnnotDesc::SEQQUALITY)].flush();
    fieldWriter[static_cast<size_t>(AnnotDesc::MAPNUMQUALITY0)].flush();
    fieldWriter[static_cast<size_t>(AnnotDesc::FILTER)].flush();
    fieldWriter[static_cast<size_t>(AnnotDesc::LINKNAME)].flush();
    fieldWriter[static_cast<size_t>(AnnotDesc::LINKID)].flush();
}

//TODO @Yeremia: Check if by default Genie should compiled with C++14
uint8_t VariantSiteParser::AlternTranslate(char alt) const {
    if (alt == 'A') return 0b000;
    if (alt == 'T') return 0b011;
    if (alt == 'G') return 0b010;
    if (alt == 'C') return 0b001;
    if (alt == 'N') return 0b100;
    return 0b111;
}

std::vector<uint8_t> VariantSiteParser::FilterTranslate(const std::string& filter) {
    if (filter.empty()) return std::vector<uint8_t>{1};
    if (filter == "PASS") return std::vector<uint8_t>{0};
    std::vector<uint8_t> filters;
    std::stringstream input(filter);
    std::string item;
    while (getline(input, item, ';')) {
        filters.push_back(static_cast<uint8_t>(std::stoi(item)) + 2);
    }
    filters.push_back(0xFF);
    return filters;
}

}  // namespace variant_site
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
