/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>

#include "genie/core/arrayType.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"
#include "genie/variantsite/Tiles.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {

void TiledStream::write(std::string value) {
    setTile();
    tiles.tileWriter.back().write(value);
    tiles.tileWriter.back().write(0, 8);
}

void TiledStream::setTile() {
    if (rowsPerTile == 0) {
    }
    else if (rowInTile < rowsPerTile) {
        rowInTile++;
    }
    else {
        tiles.tileWriter.back().flush();
        tiles.tileData.emplace_back("");
        tiles.tileWriter.emplace_back(&tiles.tileData.back());
        rowInTile = 1;
    }
}

}  // namespace variant_site
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
