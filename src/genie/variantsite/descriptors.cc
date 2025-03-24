/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/variantsite/descriptors.h"

#include <algorithm>
#include <string>
#include <utility>

#include "genie/core/arrayType.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {

void Descriptors::write(genie::core::record::variant_site::Record variantSite) {
  tiles[genie::core::AnnotDesc::SEQUENCEID].write(variantSite.getSeqID(), 16);
  tiles[genie::core::AnnotDesc::STARTPOS].write(variantSite.getPos(), 64);
  tiles[genie::core::AnnotDesc::STRAND].write(variantSite.getStrand(), 2);
  tiles[genie::core::AnnotDesc::NAME].write(variantSite.getID());
  tiles[genie::core::AnnotDesc::DESCRIPTION].write(
      variantSite.getDescription());

  std::vector<uint8_t> reference;
  for (const auto& ref : variantSite.getRef())
    reference.emplace_back(AlternTranslate(ref));
  reference.emplace_back(alternEnd);
  tiles[genie::core::AnnotDesc::REFERENCE].write(reference, 3);

  const auto& altArray = variantSite.getAlt();
  // look for <DEL>
  bool foundDel = false;
  for (auto i = 0; i < variantSite.getAltCount(); ++i) {
    const auto& altern = altArray[i];
    if (!altern.find("<DEL>")) {
      foundDel = true;
    }
  }

  std::vector<uint8_t> altern;
  if (!foundDel) {
    for (auto i = 0; i < variantSite.getAltCount(); ++i) {
      const auto& altString = altArray[i];
      for (const auto& alt : altString) {
        altern.emplace_back(AlternTranslate(alt));
      }
      if (i + 1 < variantSite.getAltCount()) altern.emplace_back(alternEndLine);
    }
  }
  altern.emplace_back(alternEnd);
  tiles[genie::core::AnnotDesc::ALTERN].write(altern, 3);

  tiles[genie::core::AnnotDesc::DEPTH].write(variantSite.getDepth(), 32);
  tiles[genie::core::AnnotDesc::SEQQUALITY].write(variantSite.getSeqQual(), 32);
  tiles[genie::core::AnnotDesc::MAPQUALITY].write(variantSite.getMapQual(), 32);
  tiles[genie::core::AnnotDesc::MAPNUMQUALITY0].write(
      variantSite.getMapNumQual0(), 32);
  auto filters = FilterTranslate(variantSite.getFilters());
  tiles[genie::core::AnnotDesc::FILTER].write(filters, 8);
  if (variantSite.isLinkedRecord()) {
    tiles[genie::core::AnnotDesc::LINKNAME].write(variantSite.getLinkName());
    tiles[genie::core::AnnotDesc::LINKID].write(variantSite.getReferenceBoxID(),
                                                8);
  } else {
    //    std::string emptystr = "";
    //    tiles[genie::core::AnnotDesc::LINKNAME].write(emptystr);

    tiles[genie::core::AnnotDesc::LINKNAME].emptyForRow();
    tiles[genie::core::AnnotDesc::LINKID].write((uint8_t)255, 8);
  }
}

void Descriptors::writeDanglingBits() {
  tiles[genie::core::AnnotDesc::SEQUENCEID].wrapUp();      // 1
  tiles[genie::core::AnnotDesc::STARTPOS].wrapUp();        // 2
  tiles[genie::core::AnnotDesc::STRAND].wrapUp();          // 4
  tiles[genie::core::AnnotDesc::NAME].wrapUp();            // 5
  tiles[genie::core::AnnotDesc::DESCRIPTION].wrapUp();     // 6
  tiles[genie::core::AnnotDesc::LINKNAME].wrapUp();        // 7
  tiles[genie::core::AnnotDesc::LINKID].wrapUp();          // 8
  tiles[genie::core::AnnotDesc::DEPTH].wrapUp();           // 9
  tiles[genie::core::AnnotDesc::SEQQUALITY].wrapUp();      // 10
  tiles[genie::core::AnnotDesc::MAPQUALITY].wrapUp();      // 11
  tiles[genie::core::AnnotDesc::MAPNUMQUALITY0].wrapUp();  // 12
  tiles[genie::core::AnnotDesc::REFERENCE].wrapUp();       // 13
  tiles[genie::core::AnnotDesc::ALTERN].wrapUp();          // 14
  tiles[genie::core::AnnotDesc::FILTER].wrapUp();          // 17
}

void Descriptors::init() {
  tiles[genie::core::AnnotDesc::SEQUENCEID].setRowsPerTile(rowsPerTile);   // 1
  tiles[genie::core::AnnotDesc::STARTPOS].setRowsPerTile(rowsPerTile);     // 2
  tiles[genie::core::AnnotDesc::STRAND].setRowsPerTile(rowsPerTile);       // 4
  tiles[genie::core::AnnotDesc::NAME].setRowsPerTile(rowsPerTile);         // 5
  tiles[genie::core::AnnotDesc::DESCRIPTION].setRowsPerTile(rowsPerTile);  // 6
  tiles[genie::core::AnnotDesc::LINKNAME].setRowsPerTile(rowsPerTile);     // 7
  tiles[genie::core::AnnotDesc::LINKID].setRowsPerTile(rowsPerTile);       // 8
  tiles[genie::core::AnnotDesc::DEPTH].setRowsPerTile(rowsPerTile);        // 9
  tiles[genie::core::AnnotDesc::SEQQUALITY].setRowsPerTile(rowsPerTile);   // 10
  tiles[genie::core::AnnotDesc::MAPQUALITY].setRowsPerTile(rowsPerTile);   // 11
  tiles[genie::core::AnnotDesc::MAPNUMQUALITY0].setRowsPerTile(
      rowsPerTile);                                                      // 12
  tiles[genie::core::AnnotDesc::REFERENCE].setRowsPerTile(rowsPerTile);  // 13
  tiles[genie::core::AnnotDesc::ALTERN].setRowsPerTile(rowsPerTile);     // 14
  tiles[genie::core::AnnotDesc::FILTER].setRowsPerTile(rowsPerTile);     // 17
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
