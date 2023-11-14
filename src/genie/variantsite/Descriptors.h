/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_VARIANT_SITE_DESCRIPTORS_H_
#define SRC_GENIE_VARIANT_SITE_DESCRIPTORS_H_

// ---------------------------------------------------------------------------------------------------------------------
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/variantsite/Tiles.h"
#include "genie/core/constants.h"
#include "genie/core/record/variant_site/record.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {

class Descriptors {
 public:
    Descriptors() : dataFields{}, fieldWriter{}, rowsPerTile(0), rowInTile(0) { init(); }
    Descriptors(uint64_t _rowsPerTile) : dataFields{}, fieldWriter{}, rowsPerTile(_rowsPerTile), rowInTile(0) {
        init();
    }

    void setTileSize(uint64_t _rowsPerTile) {
        rowsPerTile = _rowsPerTile;
        init();
    }

    void write(genie::core::record::variant_site::Record variantSite);

    std::map<genie::core::AnnotDesc, std::stringstream>& getDescriptors() { return dataFields; }
    std::map<genie::core::AnnotDesc, TiledStream>& getTiles() { return tiles; }

    void writeDanglingBits();

 private:
    const uint8_t alternEndLine = 0x06;
    const uint8_t alternEnd = 0x07;


    std::map<genie::core::AnnotDesc,TiledStream> tiles;

    std::map<genie::core::AnnotDesc, std::stringstream> dataFields;
    std::map<genie::core::AnnotDesc, genie::core::Writer> fieldWriter;
    uint64_t rowsPerTile;
    uint64_t rowInTile;

    void addWriter(genie::core::AnnotDesc id) ;

    void init();

    uint8_t AlternTranslate(char alt) const;

    static std::vector<uint8_t> FilterTranslate(const std::string& filter);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace variant_site
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_VARIANT_SITE_DESCRIPTORS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
