/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_TILESTRUCTURE_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_TILESTRUCTURE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

class TileStructure {
 private:
    bool variable_size_tiles;
    uint64_t n_tiles;
    std::vector<std::vector<uint64_t>> start_index;
    std::vector<std::vector<uint64_t>> end_index;
    std::vector<uint64_t> tile_size;
    uint8_t ATCoordSize;
    bool two_dimensional;

    uint8_t coordSizeInBits(uint8_t ATCoordSize) const;

 public:
    TileStructure(uint8_t ATCoordSize, std::vector<uint64_t> defaultTileSize);
    TileStructure(uint8_t ATCoordSize, uint64_t defaultTileSize);
    TileStructure(uint8_t ATCoordSize, bool two_dimensional, bool variable_size_tiles, uint64_t n_tiles,
                  std::vector<std::vector<uint64_t>> start_index, std::vector<std::vector<uint64_t>> end_index,
                  std::vector<uint64_t> tile_size);

    TileStructure(util::BitReader& reader, uint8_t ATCoordSize, bool two_dimensional);

    TileStructure();

    void Read(util::BitReader& reader);
    void Read(util::BitReader& reader, uint8_t ATCoordSize, bool two_dimensional);
    void Write(util::BitWriter& writer) const;

    size_t GetSize(util::BitWriter& writesize) const;

    bool isVariableSizeTiles() const { return variable_size_tiles; }
    uint64_t getNumberOfTiles() const { return n_tiles; }
    std::vector<std::vector<uint64_t>> getAllStartIndices() const { return start_index; }
    std::vector<std::vector<uint64_t>> getALLEndIndices() const { return end_index; }
    std::vector<uint64_t> getAllTileSizes() const { return tile_size; }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_TILESTRUCTURE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
