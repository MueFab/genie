/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_TRACKPROPERTY_TILES_H_
#define SRC_GENIE_TRACKPROPERTY_TILES_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <sstream>
#include <string>

#include "genie/core/constants.h"
#include "genie/variantsite/tiles.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace track_property {

// ---------------------------------------------------------------------------------------------------------------------

class Tiles {
 public:
    variant_site::TiledStream& operator[](const core::AnnotDesc& desc);
    std::map<core::AnnotDesc, std::stringstream> getTile(uint64_t i);
    std::map<core::AnnotDesc, variant_site::TiledStream>& getMap() { return tiles; }

 private:
    std::map<core::AnnotDesc, variant_site::TiledStream> tiles;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace track_property
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_TRACKPROPERTY_TILES_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
