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

void VaritanSiteParser::addWriter(DescriptorID id) {
    dataFields[id];
    Writer writer(&dataFields[id]);
    fieldWriter[static_cast<size_t>(id)] = writer;
}

void VaritanSiteParser::init() {
    std::stringstream tempStream;
    fieldWriter.resize(static_cast<size_t>(DescriptorID::FILTER) + 1, Writer(&tempStream));
    addWriter(DescriptorID::SEQUENCEID);      // 1
    addWriter(DescriptorID::STARTPOS);        // 2
    addWriter(DescriptorID::STRAND);          // 4
    addWriter(DescriptorID::NAME);            // 5
    addWriter(DescriptorID::DESCRIPTION);     // 6
    addWriter(DescriptorID::LINKNAME);        // 7
    addWriter(DescriptorID::LINKID);          // 8
    addWriter(DescriptorID::DEPTH);           // 9
    addWriter(DescriptorID::SEQQUALITY);      // 10
    addWriter(DescriptorID::MAPQUALITY);      // 11
    addWriter(DescriptorID::MAPNUMQUALITY0);  // 12
    addWriter(DescriptorID::REFERENCE);       // 13
    addWriter(DescriptorID::ALTERN);          // 14
    addWriter(DescriptorID::FILTER);          // 17
                                              // addWriter(DescriptorID::ATTRIBUTE);       // 31
    uint16_t attributeID = 0;
    for (auto infoField : infoFields) {
        Writer writer(&attributeStream[infoField]);
        attributeWriter.push_back(writer);

        AttributeData attribute(static_cast<uint8_t>(infoField.length()), infoField, attributeID);
        attributeData[infoField] = attribute;
        attributeID++;
    }
    numberOfAttributes = attributeID;
}

bool VaritanSiteParser::fillRecord(util::BitReader reader) {
    if (!variantSite.read(reader)) return false;
    return true;
}

void VaritanSiteParser::ParseOne() {
    fieldWriter[static_cast<size_t>(DescriptorID::SEQUENCEID)].write(variantSite.getSeqID(), 16);
    startPos = variantSite.getPos() - startPos;
    fieldWriter[static_cast<size_t>(DescriptorID::STARTPOS)].write(startPos, 64);
    fieldWriter[static_cast<size_t>(DescriptorID::STRAND)].write(variantSite.getStrand(), 2);
    fieldWriter[static_cast<size_t>(DescriptorID::NAME)].write(variantSite.getID());
    fieldWriter[static_cast<size_t>(DescriptorID::DESCRIPTION)].write(variantSite.getDescription());
    fieldWriter[static_cast<size_t>(DescriptorID::DESCRIPTION)].write(0, 8);

    const auto& reference = variantSite.getRef();
    for (const auto& ref : reference) {
        fieldWriter[static_cast<size_t>(DescriptorID::REFERENCE)].write(ref, 3);
    }
    fieldWriter[static_cast<size_t>(DescriptorID::ALTERN)].write(AlternEnd, 3);

    fieldWriter[static_cast<size_t>(DescriptorID::REFERENCE)].write(0, 8);

    const auto& altArray = variantSite.getAlt();
    for (auto i = 0; i < variantSite.getAltCount(); ++i) {
        const auto& altern = altArray[i];
        for (const auto& alt : altern) {
            fieldWriter[static_cast<size_t>(DescriptorID::ALTERN)].write(AlternTranslate(alt), 3);
        }
        fieldWriter[static_cast<size_t>(DescriptorID::ALTERN)].write(AlternEndLine, 3);
    }
    fieldWriter[static_cast<size_t>(DescriptorID::ALTERN)].write(AlternEnd, 3);

    fieldWriter[static_cast<size_t>(DescriptorID::DEPTH)].write(variantSite.getDepth(), 32);
    fieldWriter[static_cast<size_t>(DescriptorID::SEQQUALITY)].write(variantSite.getSeqQual(), 32);
    fieldWriter[static_cast<size_t>(DescriptorID::MAPQUALITY)].write(variantSite.getMapQual(), 32);
    fieldWriter[static_cast<size_t>(DescriptorID::MAPNUMQUALITY0)].write(variantSite.getMapNumQual0(), 32);
    fieldWriter[static_cast<size_t>(DescriptorID::FILTER)].write(variantSite.getFilters());
    fieldWriter[static_cast<size_t>(DescriptorID::FILTER)].write(0, 8);
    if (variantSite.isLinkedRecord()) {
        fieldWriter[static_cast<size_t>(DescriptorID::LINKNAME)].write(variantSite.getLinkName());
        fieldWriter[static_cast<size_t>(DescriptorID::LINKNAME)].write(0, 8);
        fieldWriter[static_cast<size_t>(DescriptorID::LINKID)].write(variantSite.getReferenceBoxID(), 8);
    } else {
        fieldWriter[static_cast<size_t>(DescriptorID::LINKID)].write(255, 8);
    }
    std::map<std::string, AttributeData>::iterator it;
    for (it = attributeData.begin(); it != attributeData.end(); it++) {
        ParseAttribute(it->first);
    }
}

void VaritanSiteParser::ParseAttribute(uint8_t index) {
    const auto infoTag = variantSite.getInfoTag()[index].info_tag;
    attributeData[infoTag].setAttributeType(variantSite.getInfoTag()[index].info_type);
    attributeData[infoTag].setArrayLength(variantSite.getInfoTag()[index].info_array_len);

    const auto& tag = variantSite.getInfoTag();
    for (const auto& value : tag[index].infoValue) {
        arrayType toval;
        toval.toFile(attributeData[infoTag].getAttributeType(), value, attributeWriter[index]);
        if (attributeData[infoTag].getAttributeType() == 0) attributeWriter[index].write(0, 8, true);
    }
}

void VaritanSiteParser::ParseAttribute(std::string infoTagfield) {
    const auto& infoArray = variantSite.getInfoTag();
    uint8_t matchLocation = static_cast<uint8_t>(infoArray.size());
    for (uint8_t i = 0; i < infoArray.size(); ++i) {
        if (infoArray[i].info_tag == infoTagfield) {
            matchLocation = i;
            break;
        }
    }
    if (matchLocation < infoArray.size()) {
        ParseAttribute(matchLocation);
    } else {  // insert default values
        arrayType def;
        auto defaultType = attributeData[infoTagfield].getAttributeType();
        auto defaultValue = def.getDefaultValue(defaultType);
        auto defaultSize = def.getDefaultBitsize(defaultType);
        for (auto i = 0; i < attributeData[infoTagfield].getArrayLength(); ++i)
            attributeWriter[attributeData[infoTagfield].getAttributeID()].write(defaultValue, defaultSize);
    }
}

void VaritanSiteParser::ParseInfoFields() {
    std::stringstream infoFieldsList;
    infoFieldsList << infoFieldsJSON.substr(1, infoFieldsJSON.size() - 2);

    std::string infoField;
    while (std::getline(infoFieldsList, infoField, ',')) {
        infoField.erase(std::remove(infoField.begin(), infoField.end(), '"'), infoField.end());
        infoField.erase(std::remove(infoField.begin(), infoField.end(), ' '), infoField.end());
        infoFields.push_back(infoField);
    }
}

void VaritanSiteParser::ParseInfoField(std::string id) {
    std::stringstream infoFieldsList;
    infoFieldsList << infoFieldsJSON.substr(1, infoFieldsJSON.size() - 2);
    // std::vector<std::string> read;
    for (std::string read; infoFieldsList.getline(&read[0], 64);) {
        if (read.size() < 3) continue;  // read = "{" or "}"
        read.erase(std::remove(read.begin(), read.end(), '"'), read.end());
        read.erase(std::remove(read.begin(), read.end(), ','), read.end());
        auto posColumn = read.find(':');
        std::string IDField = read.substr(0, posColumn - 1);
        std::string value = read.substr(posColumn + 1);
        if (IDField == "ID") {
        }
    }
}

void VaritanSiteParser::writeDanglingBits() {
    fieldWriter[static_cast<size_t>(DescriptorID::SEQUENCEID)].flush();
    fieldWriter[static_cast<size_t>(DescriptorID::STARTPOS)].flush();
    fieldWriter[static_cast<size_t>(DescriptorID::STRAND)].flush();
    fieldWriter[static_cast<size_t>(DescriptorID::NAME)].flush();
    fieldWriter[static_cast<size_t>(DescriptorID::DESCRIPTION)].flush();
    fieldWriter[static_cast<size_t>(DescriptorID::REFERENCE)].flush();
    fieldWriter[static_cast<size_t>(DescriptorID::ALTERN)].flush();
    fieldWriter[static_cast<size_t>(DescriptorID::DEPTH)].flush();
    fieldWriter[static_cast<size_t>(DescriptorID::SEQQUALITY)].flush();
    fieldWriter[static_cast<size_t>(DescriptorID::MAPNUMQUALITY0)].flush();
    fieldWriter[static_cast<size_t>(DescriptorID::FILTER)].flush();
    fieldWriter[static_cast<size_t>(DescriptorID::LINKNAME)].flush();
    fieldWriter[static_cast<size_t>(DescriptorID::LINKID)].flush();
    //   fieldWriter[static_cast<size_t>(DescriptorID::ATTRIBUTE)].flush();
}

uint8_t VaritanSiteParser::AlternTranslate(char alt) const {
    if (alt == 'A') return 0b000;
    if (alt == 'T') return 0b011;
    if (alt == 'G') return 0b010;
    if (alt == 'C') return 0b001;
    if (alt == 'N') return 0b100;
    return 0b111;
}

}  // namespace variant_site
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
