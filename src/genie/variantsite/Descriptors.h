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

#include "genie/core/constants.h"
#include "genie/core/record/variant_site/record.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
#include "genie/variantsite/Tiles.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {

class Descriptors {
 public:
    Descriptors() : rowsPerTile(0), rowInTile(0) { init(); }
    Descriptors(uint64_t _rowsPerTile) : rowsPerTile(_rowsPerTile), rowInTile(0) { init(); }

    void setTileSize(uint64_t _rowsPerTile) {
        rowsPerTile = _rowsPerTile;
        init();
    }

    void write(genie::core::record::variant_site::Record variantSite);

    std::map<genie::core::AnnotDesc, TiledStream>& getTiles() { return tiles; }

    void writeDanglingBits();

 private:
    const uint8_t alternEndLine = 0x06;
    const uint8_t alternEnd = 0x07;

    std::map<genie::core::AnnotDesc, TiledStream> tiles;

    uint64_t rowsPerTile;
    uint64_t rowInTile;

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
