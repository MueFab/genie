/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#include "TileStructure.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

uint8_t TileStructure::coordSizeInBits(uint8_t size) const {
    switch (size) {
        case 0:
            return 8;
        case 1:
            return 16;
        case 2:
            return 32;
        case 3:
            return 64;
        default:
            return 0;
    }
}

TileStructure::TileStructure()
    : variable_size_tiles(false),
      n_tiles(0),
      start_index{},
      end_index{},
      tile_size{0},
      ATCoordSize(0),
      two_dimensional(false) {}

TileStructure::TileStructure(util::BitReader& reader, uint8_t ATCoordSize, bool two_dimensional)
    : ATCoordSize(ATCoordSize), two_dimensional(two_dimensional) {
    read(reader);
}

TileStructure::TileStructure(uint8_t ATCoordSize, uint64_t defaultTileSize)
    : TileStructure(ATCoordSize, {defaultTileSize, 0}) {}

TileStructure::TileStructure(uint8_t ATCoordSize, std::vector<uint64_t> defaultTileSize)

    : variable_size_tiles(false),
      n_tiles(1),
      start_index{},
      end_index{},
      tile_size(defaultTileSize),
      ATCoordSize(ATCoordSize),
      two_dimensional(two_dimensional) {
    if (defaultTileSize.at(1) == 0)
        two_dimensional = false;
    else
        two_dimensional = true;
}

TileStructure::TileStructure(uint8_t ATCoordSize, bool two_dimensional, bool variable_size_tiles, uint64_t n_tiles,
                             std::vector<std::vector<uint64_t>> start_index,
                             std::vector<std::vector<uint64_t>> end_index, std::vector<uint64_t> tile_size)
    : variable_size_tiles(variable_size_tiles),
      n_tiles(n_tiles),
      start_index(start_index),
      end_index(end_index),
      tile_size(tile_size),
      ATCoordSize(ATCoordSize),
      two_dimensional(two_dimensional) {
    if (!variable_size_tiles) {
        this->start_index.clear();
        this->end_index.clear();
    } else {
        this->tile_size.clear();
    }
}

void TileStructure::read(util::BitReader& reader, uint8_t AT_coordsize, bool twoDimensional) {
    this->ATCoordSize = AT_coordsize;
    this->two_dimensional = twoDimensional;
    read(reader);
}

void TileStructure::read(util::BitReader& reader) {
    reader.read_b(7);
    variable_size_tiles = static_cast<bool>(reader.read_b(1));
    n_tiles = static_cast<uint64_t>(reader.read_b(coordSizeInBits(ATCoordSize)));
    auto dimensions = two_dimensional ? 2 : 1;
    if (variable_size_tiles) {
        tile_size.clear();
        start_index.resize(n_tiles, std::vector<uint64_t>(dimensions));
        end_index.resize(n_tiles, std::vector<uint64_t>(dimensions));
        for (uint64_t i = 0; i < n_tiles; ++i) {
            for (auto j = 0; j < dimensions; ++j) {
                start_index[i][j] = static_cast<uint64_t>(reader.read_b(coordSizeInBits(ATCoordSize)));
                end_index[i][j] = static_cast<uint64_t>(reader.read_b(coordSizeInBits(ATCoordSize)));
            }
        }
    } else {
        end_index.clear();
        start_index.clear();
        tile_size.resize(dimensions);
        for (auto& dimension : tile_size)
            dimension = static_cast<uint64_t>(reader.read_b(coordSizeInBits(ATCoordSize)));
    }
}

void TileStructure::write(core::Writer& writer) const {
    writer.write_reserved(7);
    writer.write(variable_size_tiles, 1);
    writer.write(n_tiles, coordSizeInBits(ATCoordSize));

    auto dimensions = two_dimensional ? 2 : 1;
    if (variable_size_tiles) {
        for (uint64_t i = 0; i < n_tiles; ++i)
            for (auto j = 0; j < dimensions; ++j) {
                writer.write(start_index[i][j], coordSizeInBits(ATCoordSize));
                writer.write(end_index[i][j], coordSizeInBits(ATCoordSize));
            }
    } else {
        for (auto j = 0; j < dimensions; ++j) writer.write(tile_size[j], coordSizeInBits(ATCoordSize));
    }
}

size_t TileStructure::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.getBitsWritten();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
