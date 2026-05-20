/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_TRACK_DESCRIPTORS_H_
#define SRC_GENIE_TRACK_DESCRIPTORS_H_

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
#include "genie/core/track_record/record.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/track/tiles.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace track {

class Descriptors {
 public:
    Descriptors() : rowsPerTile(0), rowInTile(0) { init(); }
    explicit Descriptors(uint64_t _rowsPerTile) : rowsPerTile(_rowsPerTile), rowInTile(0) { init(); }

    void setTileSize(uint64_t _rowsPerTile) {
        rowsPerTile = _rowsPerTile;
        init();
    }

    void write(genie::core::record::track::Record trackRecord);

    std::map<genie::core::AnnotDesc, TiledStream>& getTiles() { return tiles; }

    void writeDanglingBits();

 private:
    std::map<genie::core::AnnotDesc, TiledStream> tiles;

    uint64_t rowsPerTile;
    uint64_t rowInTile;

    void init();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace track
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_TRACK_DESCRIPTORS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
