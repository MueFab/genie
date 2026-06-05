/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_VARIANTSITE_TILES_H_
#define SRC_GENIE_VARIANTSITE_TILES_H_

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
#include "genie/core/record/variant/record.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {

class Tiles {
 public:
    Tiles() {
        tileData.emplace_back("");
        tileWriter.emplace_back(&tileData.back());
    }
    std::vector<std::stringstream> tileData;
    std::vector<util::BitWriter> tileWriter;
};

class TiledStream {
 public:
    explicit TiledStream(uint64_t _rowsPerTile) : rowsPerTile(_rowsPerTile), rowInTile(0), tiles{} {}
    TiledStream() : rowsPerTile(0), rowInTile(0), tiles{} {}

    void setRowsPerTile(uint64_t _rowsPerTile) {
        rowsPerTile = _rowsPerTile;
        rowInTile = 0;
    }

    template <class T>
    void write(T value, uint8_t bits) {
        setTile();
        if (bits == 0) bits = sizeof(T);
        tiles.tileWriter.back().WriteBits(value, bits);
    }

    template <class T>
    void write(std::vector<T> values, uint8_t bits) {
        setTile();
        if (bits == 0) bits = sizeof(T);
        for (auto value : values) tiles.tileWriter.back().WriteBits(value, bits);
    }

    void write(std::string value);

    void emptyForRow() { setTile(); }

    void wrapUp() { tiles.tileWriter.back().FlushBits(); }
    std::vector<std::stringstream>& getTiles() { return tiles.tileData; }
    size_t getNrOfTiles() { return tiles.tileData.size(); }
    std::stringstream& getTile(uint64_t tilenr) { return tiles.tileData.at(tilenr); }

    size_t getBitsWrittenInTile(size_t tilenr) { return tiles.tileWriter.at(tilenr).GetTotalBitsWritten(); }

 private:
    uint64_t rowsPerTile;
    uint64_t rowInTile;
    Tiles tiles;

    void setTile();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace variant_site
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_VARIANTSITE_TILES_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
