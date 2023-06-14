/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>

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

    for (auto i = 0; i < variantSite.getAltCount(); ++i) {
        auto altern = variantSite.getAlt()[i];
        for (auto alt : altern) {
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
    for (auto i = 0; i < variantSite.getInfoCount(); ++i) {
        fieldWriter[static_cast<size_t>(DescriptorID::ATTRIBUTE)].write(variantSite.getInfoTag()[i].info_tag);  // ???
        for (auto value : variantSite.getInfoTag()[i].info_value) {
            fieldWriter[static_cast<size_t>(DescriptorID::ATTRIBUTE)].write(value);
            if (variantSite.getInfoTag()[i].info_type == 0)
                fieldWriter[static_cast<size_t>(DescriptorID::ATTRIBUTE)].write(0, 8);
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

uint8_t VaritanSiteParser::AlternTranslate(char alt) {
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
