/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_TILESTRUCTURE_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_TILESTRUCTURE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#include "genie/core/writer.h"
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
    TileStructure(uint8_t ATCoordSize, bool two_dimensional);
    TileStructure(uint8_t ATCoordSize, bool two_dimensional, bool variable_size_tiles, uint64_t n_tiles,
                  std::vector<std::vector<uint64_t>> start_index, std::vector<std::vector<uint64_t>> end_index,
                  std::vector<uint64_t> tile_size);

    TileStructure(util::BitReader& reader, uint8_t ATCoordSize, bool two_dimensional);

    TileStructure();

    void read(util::BitReader& reader);
    void read(util::BitReader& reader, uint8_t ATCoordSize, bool two_dimensional);
    void write(core::Writer& writer) const;

    size_t getSize(core::Writer& writesize) const;

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
