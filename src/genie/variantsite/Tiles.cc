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
    } else if (rowInTile < rowsPerTile) {
        rowInTile++;
    } else {
        tiles.tileWriter.back().flush();
        tiles.tileData.emplace_back("");
        tiles.tileWriter.emplace_back(&tiles.tileData.back());
        rowInTile = 0;
    }
}

}  // namespace variant_site
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
