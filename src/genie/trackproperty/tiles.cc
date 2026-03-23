/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/trackproperty/tiles.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace track_property {

// ---------------------------------------------------------------------------------------------------------------------

variant_site::Tile& Tiles::operator[](const core::AnnotDesc& desc) {
    return tiles[desc];
}

// ---------------------------------------------------------------------------------------------------------------------

std::map<core::AnnotDesc, std::stringstream> Tiles::getTile(uint64_t i) {
    std::map<core::AnnotDesc, std::stringstream> finalstreams;
    for (auto& tile : tiles) {
        finalstreams[tile.first] << tile.second.getTile(i).rdbuf();
    }
    return finalstreams;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace track_property
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
