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
    fieldWriter.resize(static_cast<size_t>(DescriptorID::ATTRIBUTE) + 1, Writer(&tempStream));
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
    addWriter(DescriptorID::ATTRIBUTE);       // 31
}

bool VaritanSiteParser::fillRecord(util::BitReader reader) {
    if (!variantSite.read(reader)) return false;
    return true;
}

void VaritanSiteParser::ParseOne() {
    fieldWriter[static_cast<size_t>(DescriptorID::SEQUENCEID)].write(variantSite.getSeqID(), 16);
    fieldWriter[static_cast<size_t>(DescriptorID::STARTPOS)].write(variantSite.getPos(), 40);
    fieldWriter[static_cast<size_t>(DescriptorID::STRAND)].write(variantSite.getStrand(), 8);
    fieldWriter[static_cast<size_t>(DescriptorID::NAME)].write(variantSite.getID());
    fieldWriter[static_cast<size_t>(DescriptorID::DESCRIPTION)].write(variantSite.getDescription());
    fieldWriter[static_cast<size_t>(DescriptorID::DESCRIPTION)].write(0, 8);
    fieldWriter[static_cast<size_t>(DescriptorID::REFERENCE)].write(variantSite.getRef());
    fieldWriter[static_cast<size_t>(DescriptorID::REFERENCE)].write(0, 8);
    const auto& altArray = variantSite.getAlt();

    for (auto i = 0; i < variantSite.getAltCount(); ++i) {
        const auto& altern = altArray[i];
        for (const auto& alt : altern) {
            fieldWriter[static_cast<size_t>(DescriptorID::ALTERN)].write(AlternTranslate(alt), 3);
        }
    }

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
    }

    if (attributeInfo.size() == 0) {
        AttributeData temp;
        auto size = variantSite.getInfoCount();
        attributeInfo.resize(size, temp);
        const auto& infoTag = variantSite.getInfoTag();
        for (auto i = 0; i < size; ++i) {
            const auto& info = infoTag[i];
            AttributeData data(info.info_tag_len, info.info_tag, info.info_type, info.info_array_len);
            attributeInfo[i] = data;
        }
    }

    for (uint8_t i = 0; i < attributeInfo.size(); ++i) {
        ParseAttribute(i);
    }
}

void VaritanSiteParser::ParseAttribute(uint8_t index) {
    Writer writer(&attributeInfo[index].getValue());
    const auto& infoTag = variantSite.getInfoTag();

    if (index < variantSite.getInfoCount()) {
        const auto& valueArray = infoTag[index].info_value;
        for (const auto& value : valueArray) {
            writer.write(value);
            if (variantSite.getInfoTag()[index].info_type == 0) writer.write(0, 8);
        }
    } else {
        arrayType typeDefault;
        auto bitSize = typeDefault.getDefaultBitsize(attributeInfo[index].getAttributeType());
        auto value = typeDefault.getDefaultValue(attributeInfo[index].getAttributeType());

        for (auto j = 0; j < attributeInfo[index].getArrayLength(); ++j) {
            writer.write(value, bitSize);
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
    fieldWriter[static_cast<size_t>(DescriptorID::ATTRIBUTE)].flush();
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
