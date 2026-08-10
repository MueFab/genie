/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_TRACKPROPERTY_DESCRIPTORS_H_
#define SRC_GENIE_TRACKPROPERTY_DESCRIPTORS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>

#include "genie/core/record/track_property/record.h"
#include "genie/core/constants.h"
#include "genie/variantsite/tiles.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::track_property {

// ---------------------------------------------------------------------------------------------------------------------

class Descriptors {
 public:
    Descriptors() : rowsPerTile(0) { init(); }
    explicit Descriptors(uint64_t _rowsPerTile) : rowsPerTile(_rowsPerTile) { init(); }

    void setTileSize(uint64_t _rowsPerTile) {
        rowsPerTile = _rowsPerTile;
        init();
    }

    void write(genie::core::record::track_property::Record trackPropertyRecord);
    void init();
    void writeDanglingBits();
    std::map<genie::core::AnnotDesc, variant_site::TiledStream>& getTiles() { return tiles; }

 private:
    uint64_t rowsPerTile;
    std::map<genie::core::AnnotDesc, variant_site::TiledStream> tiles;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::track_property

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_TRACKPROPERTY_DESCRIPTORS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
