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
    : variable_size_tiles(false), n_tiles(0), start_index{}, end_index{}, tile_size{0}, two_dimensional(false) {}

TileStructure::TileStructure(util::BitReader& reader, uint8_t ATCoordSize, bool two_dimensional)
    : ATCoordSize(ATCoordSize), two_dimensional(two_dimensional) {
    read(reader);
}
TileStructure::TileStructure(uint8_t ATCoordSize, bool two_dimensional, bool variable_size_tiles, uint64_t n_tiles,
                             std::vector<std::vector<uint64_t>> start_index,
                             std::vector<std::vector<uint64_t>> end_index, std::vector<uint64_t> tile_size)
    : ATCoordSize(ATCoordSize),
      two_dimensional(two_dimensional),
      variable_size_tiles(variable_size_tiles),
      n_tiles(n_tiles),
      start_index(start_index),
      end_index(end_index),
      tile_size(tile_size) {}

void TileStructure::read(util::BitReader& reader, uint8_t AT_coordsize, bool twoDimensional) {
    this->ATCoordSize = AT_coordsize;
    if (!two_dimensional && twoDimensional) tile_size.resize(2);
    this->two_dimensional = twoDimensional;
    read(reader);
}

void TileStructure::read(util::BitReader& reader) {
    reader.read_b(7);
    variable_size_tiles = static_cast<bool>(reader.read_b(1));
    n_tiles = static_cast<uint64_t>(reader.read_b(coordSizeInBits(ATCoordSize)));
    auto dimensions = two_dimensional ? 2 : 1;
    if (variable_size_tiles) {
        start_index.resize(n_tiles, std::vector<uint64_t>(dimensions));
        end_index.resize(n_tiles, std::vector<uint64_t>(dimensions));
        for (auto i = 0; i < n_tiles; ++i) {
            for (auto j = 0; j < dimensions; ++j) {
                start_index[i][j] = static_cast<uint64_t>(reader.read_b(coordSizeInBits(ATCoordSize)));
                end_index[i][j] = static_cast<uint64_t>(reader.read_b(coordSizeInBits(ATCoordSize)));
            }
        }
    } else {
        tile_size.resize(two_dimensional);
        for (auto& dimension : tile_size)
            dimension = static_cast<uint64_t>(reader.read_b(coordSizeInBits(ATCoordSize)));
    }
}

void TileStructure::write(std::ostream& outputfile, bool skipEmbeddedRecord) const {
    if (!skipEmbeddedRecord) write(outputfile);
}

void TileStructure::write(std::ostream& outputfile) const {
    outputfile << std::to_string(variable_size_tiles) << ",";
    outputfile << std::to_string(n_tiles) << ",";
    auto dimensions = two_dimensional ? 2 : 1;
    if (variable_size_tiles) {
        for (auto i = 0; i < n_tiles; ++i) {
            for (auto j = 0; j < dimensions; ++j) {
                outputfile << std::to_string(start_index[i][j]) << ",";
                outputfile << std::to_string(end_index[i][j]) << ",";
            }
        }
    } else {
        for (auto j = 0; j < dimensions; ++j) outputfile << std::to_string(tile_size[j]) << ",";
    }
}

void TileStructure::write(util::BitWriter& writer, bool skipEmbeddedRecord) const {
    if (!skipEmbeddedRecord) write(writer);
}

void TileStructure::write(util::BitWriter& writer) const {
    writer.write(0, 7);
    writer.write(variable_size_tiles, 1);
    writer.write(n_tiles, coordSizeInBits(ATCoordSize));
    auto dimensions = two_dimensional ? 2 : 1;
    if (variable_size_tiles) {
        for (auto i = 0; i < n_tiles; ++i)
            for (auto j = 0; j < dimensions; ++j) {
                writer.write(start_index[i][j], coordSizeInBits(ATCoordSize));
                writer.write(end_index[i][j], coordSizeInBits(ATCoordSize));
            }
    } else {
        for (auto j = 0; j < dimensions; ++j) writer.write(tile_size[j], coordSizeInBits(ATCoordSize));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
