/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>

#include "genie/core/array_type.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"
#include "genie/functionalannotation/tiles.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace functional_annotation {

void TiledStream::write(std::string value) {
    setTile();
    for (char c : value) {
        tiles.tileWriter.back().WriteBits(static_cast<uint8_t>(c), 8);
    }
    tiles.tileWriter.back().WriteBits(0, 8);  // string-terminator
}

void TiledStream::setTile() {
    if (rowsPerTile == 0) {
    } else if (rowInTile < rowsPerTile) {
        rowInTile++;
    } else {
        tiles.tileWriter.back().FlushBits();
        tiles.tileData.emplace_back("");
        tiles.tileWriter.emplace_back(tiles.tileData.back());
        rowInTile = 1;
    }
}

}  // namespace functional_annotation
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
