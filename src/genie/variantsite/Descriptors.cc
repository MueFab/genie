/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>

#include "genie/core/arrayType.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
#include "genie/variantsite/Descriptors.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {



void Descriptors::write(genie::core::record::variant_site::Record variantSite) {
    tiles[genie::core::AnnotDesc::SEQUENCEID].write(variantSite.getSeqID(), 16);
    tiles[genie::core::AnnotDesc::STARTPOS].write(variantSite.getPos(), 64);
    tiles[genie::core::AnnotDesc::STRAND].write(variantSite.getStrand(), 2);
    tiles[genie::core::AnnotDesc::NAME].write(variantSite.getID());
    tiles[genie::core::AnnotDesc::DESCRIPTION].write(variantSite.getDescription());

    std::vector<uint8_t> reference;
    for (auto ref:variantSite.getRef())
        reference.emplace_back(ref);
    reference.emplace_back(alternEnd);
    tiles[genie::core::AnnotDesc::REFERENCE].write(reference, 3);

    std::vector<uint8_t> altern;
    for (auto altString : variantSite.getAlt()) {
        for (auto byte : altString) altern.emplace_back(AlternTranslate(byte));
        altern.emplace_back(alternEndLine);
    }
    altern.emplace_back(alternEnd);

    tiles[genie::core::AnnotDesc::ALTERN].write(altern, 3);
    tiles[genie::core::AnnotDesc::DEPTH].write(variantSite.getDepth(), 32);
    tiles[genie::core::AnnotDesc::SEQQUALITY].write(variantSite.getSeqQual(), 32);
    tiles[genie::core::AnnotDesc::MAPQUALITY].write(variantSite.getMapQual(), 32);
    tiles[genie::core::AnnotDesc::MAPNUMQUALITY0].write(variantSite.getMapNumQual0(), 32);
    auto filters = FilterTranslate(variantSite.getFilters());
    tiles[genie::core::AnnotDesc::FILTER].write(filters, 8);
    if (variantSite.isLinkedRecord()) {
        tiles[genie::core::AnnotDesc::LINKNAME].write(variantSite.getLinkName());
        tiles[genie::core::AnnotDesc::LINKID].write(variantSite.getReferenceBoxID(), 8);
    } else {
        tiles[genie::core::AnnotDesc::LINKNAME].emptyForRow();
        tiles[genie::core::AnnotDesc::LINKID].write((uint8_t)255, 8);
    }

    //---------------------------------------------

    fieldWriter[genie::core::AnnotDesc::SEQUENCEID].write(variantSite.getSeqID(), 16);
    fieldWriter[genie::core::AnnotDesc::STARTPOS].write(variantSite.getPos(), 64);

    fieldWriter[genie::core::AnnotDesc::STRAND].write(variantSite.getStrand(), 2);
    fieldWriter[genie::core::AnnotDesc::NAME].write(variantSite.getID());
    fieldWriter[genie::core::AnnotDesc::NAME].write(0, 8);
    fieldWriter[genie::core::AnnotDesc::DESCRIPTION].write(variantSite.getDescription());
    fieldWriter[genie::core::AnnotDesc::DESCRIPTION].write(0, 8);

    const auto& reference1 = variantSite.getRef();
    for (const auto& ref : reference1) {
        fieldWriter[genie::core::AnnotDesc::REFERENCE].write(AlternTranslate(ref), 3);
    }
    fieldWriter[genie::core::AnnotDesc::REFERENCE].write(alternEnd, 3);

    const auto& altArray = variantSite.getAlt();
    // look for <DEL>
    bool foundDel = false;
    for (auto i = 0; i < variantSite.getAltCount(); ++i) {
        const auto& altern1 = altArray[i];
        if (!altern1.find("<DEL>")) {
            foundDel = true;
        }
    }

    if (!foundDel) {
        for (auto i = 0; i < variantSite.getAltCount(); ++i) {
            const auto& altern1 = altArray[i];
            for (const auto& alt : altern1) {
                fieldWriter[genie::core::AnnotDesc::ALTERN].write(AlternTranslate(alt), 3);
            }
            fieldWriter[genie::core::AnnotDesc::ALTERN].write(alternEndLine, 3);
        }
    }
    fieldWriter[genie::core::AnnotDesc::ALTERN].write(alternEnd, 3);

    fieldWriter[genie::core::AnnotDesc::DEPTH].write(variantSite.getDepth(), 32);
    fieldWriter[genie::core::AnnotDesc::SEQQUALITY].write(variantSite.getSeqQual(), 32);
    fieldWriter[genie::core::AnnotDesc::MAPQUALITY].write(variantSite.getMapQual(), 32);
    fieldWriter[genie::core::AnnotDesc::MAPNUMQUALITY0].write(variantSite.getMapNumQual0(), 32);
    auto filters1 = FilterTranslate(variantSite.getFilters());
    for (auto filter : filters1) fieldWriter[genie::core::AnnotDesc::FILTER].write(filter, 8);
    if (variantSite.isLinkedRecord()) {
        fieldWriter[genie::core::AnnotDesc::LINKNAME].write(variantSite.getLinkName());
        fieldWriter[genie::core::AnnotDesc::LINKNAME].write(0, 8);
        fieldWriter[genie::core::AnnotDesc::LINKID].write(variantSite.getReferenceBoxID(), 8);
    } else {
        fieldWriter[genie::core::AnnotDesc::LINKID].write(255, 8);
    }
}

void Descriptors::writeDanglingBits() {
    fieldWriter[genie::core::AnnotDesc::SEQUENCEID].flush();
    fieldWriter[genie::core::AnnotDesc::STARTPOS].flush();
    fieldWriter[genie::core::AnnotDesc::STRAND].flush();
    fieldWriter[genie::core::AnnotDesc::NAME].flush();
    fieldWriter[genie::core::AnnotDesc::DESCRIPTION].flush();
    fieldWriter[genie::core::AnnotDesc::REFERENCE].flush();
    fieldWriter[genie::core::AnnotDesc::ALTERN].flush();
    fieldWriter[genie::core::AnnotDesc::DEPTH].flush();
    fieldWriter[genie::core::AnnotDesc::SEQQUALITY].flush();
    fieldWriter[genie::core::AnnotDesc::MAPNUMQUALITY0].flush();
    fieldWriter[genie::core::AnnotDesc::FILTER].flush();
    fieldWriter[genie::core::AnnotDesc::LINKNAME].flush();
    fieldWriter[genie::core::AnnotDesc::LINKID].flush();


    tiles[genie::core::AnnotDesc::SEQUENCEID].wrapUp();      // 1
    tiles[genie::core::AnnotDesc::STARTPOS].wrapUp();                         // 2
    tiles[genie::core::AnnotDesc::STRAND].wrapUp();          // 4
    tiles[genie::core::AnnotDesc::NAME].wrapUp();                             // 5
    tiles[genie::core::AnnotDesc::DESCRIPTION].wrapUp();     // 6
    tiles[genie::core::AnnotDesc::LINKNAME].wrapUp();                         // 7
    tiles[genie::core::AnnotDesc::LINKID].wrapUp();          // 8
    tiles[genie::core::AnnotDesc::DEPTH].wrapUp();                            // 9
    tiles[genie::core::AnnotDesc::SEQQUALITY].wrapUp();      // 10
    tiles[genie::core::AnnotDesc::MAPQUALITY].wrapUp();                       // 11
    tiles[genie::core::AnnotDesc::MAPNUMQUALITY0].wrapUp();  // 12
    tiles[genie::core::AnnotDesc::REFERENCE].wrapUp();                        // 13
    tiles[genie::core::AnnotDesc::ALTERN].wrapUp();          // 14
    tiles[genie::core::AnnotDesc::FILTER].wrapUp();                           // 17
}

void Descriptors::addWriter(genie::core::AnnotDesc id) {
    dataFields[id];
    fieldWriter[id] = genie::core::Writer(&dataFields[id]);
}

void Descriptors::init() {
    tiles[genie::core::AnnotDesc::SEQUENCEID].setRowsPerTile(rowsPerTile);  // 1
    tiles[genie::core::AnnotDesc::STARTPOS].setRowsPerTile(rowsPerTile);    // 2
    tiles[genie::core::AnnotDesc::STRAND].setRowsPerTile(rowsPerTile);      // 4
    tiles[genie::core::AnnotDesc::NAME].setRowsPerTile(rowsPerTile);        // 5
    tiles[genie::core::AnnotDesc::DESCRIPTION].setRowsPerTile(rowsPerTile);  // 6
    tiles[genie::core::AnnotDesc::LINKNAME].setRowsPerTile(rowsPerTile);     // 7
    tiles[genie::core::AnnotDesc::LINKID].setRowsPerTile(rowsPerTile);       // 8
    tiles[genie::core::AnnotDesc::DEPTH].setRowsPerTile(rowsPerTile);        // 9
    tiles[genie::core::AnnotDesc::SEQQUALITY].setRowsPerTile(rowsPerTile);   // 10
    tiles[genie::core::AnnotDesc::MAPQUALITY].setRowsPerTile(rowsPerTile);   // 11
    tiles[genie::core::AnnotDesc::MAPNUMQUALITY0].setRowsPerTile(rowsPerTile);  // 12
    tiles[genie::core::AnnotDesc::REFERENCE].setRowsPerTile(rowsPerTile);       // 13
    tiles[genie::core::AnnotDesc::ALTERN].setRowsPerTile(rowsPerTile);          // 14
    tiles[genie::core::AnnotDesc::FILTER].setRowsPerTile(rowsPerTile);          // 17

    addWriter(genie::core::AnnotDesc::SEQUENCEID);      // 1
    addWriter(genie::core::AnnotDesc::STARTPOS);        // 2
    addWriter(genie::core::AnnotDesc::STRAND);          // 4
    addWriter(genie::core::AnnotDesc::NAME);            // 5
    addWriter(genie::core::AnnotDesc::DESCRIPTION);     // 6
    addWriter(genie::core::AnnotDesc::LINKNAME);        // 7
    addWriter(genie::core::AnnotDesc::LINKID);          // 8
    addWriter(genie::core::AnnotDesc::DEPTH);           // 9
    addWriter(genie::core::AnnotDesc::SEQQUALITY);      // 10
    addWriter(genie::core::AnnotDesc::MAPQUALITY);      // 11
    addWriter(genie::core::AnnotDesc::MAPNUMQUALITY0);  // 12
    addWriter(genie::core::AnnotDesc::REFERENCE);       // 13
    addWriter(genie::core::AnnotDesc::ALTERN);          // 14
    addWriter(genie::core::AnnotDesc::FILTER);          // 17
}

uint8_t Descriptors::AlternTranslate(char alt) const {
    if (alt == 'A') return 0b000;
    if (alt == 'T') return 0b011;
    if (alt == 'G') return 0b010;
    if (alt == 'C') return 0b001;
    if (alt == 'N') return 0b100;
    return 0b101;
}

std::vector<uint8_t> Descriptors::FilterTranslate(const std::string& filter) {
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
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
