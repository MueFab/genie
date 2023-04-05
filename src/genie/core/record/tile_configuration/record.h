/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_TILE_CONFIGURATION_H_
#define SRC_GENIE_CORE_RECORD_TILE_CONFIGURATION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace tile_configuration {

class TileStructure {
 private:
    bool variable_size_tiles;
    uint64_t n_tiles;
    std::vector<std::vector<uint64_t>> start_index;
    std::vector<std::vector<uint64_t>> end_index;
    std::vector<uint64_t> tile_size;

    uint64_t readVariableSize(util::BitReader& reader, uint8_t ATCoordSize);

 public:
    TileStructure(bool variable_size_tiles, uint64_t n_tiles, std::vector<std::vector<uint64_t>> start_index,
                  std::vector<std::vector<uint64_t>> end_index, std::vector<uint64_t> tile_size);

    TileStructure(util::BitReader& reader, uint8_t ATCoordSize, bool two_dimensional);

    TileStructure();

    void read(util::BitReader& reader, uint8_t ATCoordSize, bool two_dimensional);
    void write(std::ostream& outputfile, bool two_dimensional);

    bool isVariableSizeTiles() const { return variable_size_tiles; }
    uint64_t getNumberOfTiles() const { return n_tiles; }
    std::vector<std::vector<uint64_t>> getAllStartIndices() const { return start_index; }
    std::vector<std::vector<uint64_t>> getALLEndIndices() const { return end_index; }
    std::vector<uint64_t> getAllTileSizes() const { return tile_size; }
};

/**
 *  @brief
 */
class Record {
 private:
    uint8_t AG_class;
    bool attribute_contiguity;
    bool two_dimensional;

    bool column_major_tile_order;
    uint8_t symmetry_mode;
    bool symmetry_minor_diagonal;

    bool attribute_dependent_tiles;
    TileStructure default_tile_structure;
    uint16_t n_add_tile_structures;
    std::vector<uint16_t> n_attributes;
    std::vector<std::vector<uint16_t>> attribute_ID;
    std::vector<uint8_t> n_descriptors;
    std::vector<std::vector<uint8_t>> descriptor_ID;
    std::vector<TileStructure> additional_tile_structure;

 public:
    /**
     * @brief
     */
    Record();
    Record(util::BitReader& reader, uint8_t ATCoordSize);
    Record(uint8_t AG_class, bool attribute_contiguity, bool two_dimensional,
           bool column_major_tile_order, uint8_t symmetry_mode, bool symmetry_minor_diagonal,
           bool attribute_dependent_tiles, TileStructure default_tile_structure, uint16_t n_add_tile_structures,
           std::vector<uint16_t> n_attributes, std::vector<std::vector<uint16_t>> attribute_ID,
           std::vector<uint8_t> n_descriptors, std::vector<std::vector<uint8_t>> descriptor_ID,
           std::vector<TileStructure> additional_tile_structure);

    void read(util::BitReader& reader, uint8_t ATCoordSize);
    void write(std::ostream& outputfile);

    uint8_t getAttributeGroupClass() const { return AG_class; }
    bool isAttributeContiguity() const { return attribute_contiguity; }
    bool isTwoDimensional() const { return two_dimensional; }
    bool isColumnMajorTileOrder() const { return column_major_tile_order; }
    uint8_t getSymetryMode() const { return symmetry_mode; }
    bool isSymetryMinorDiagonal() const { return symmetry_minor_diagonal; }

    bool isAttrributeDependentTiles() const { return attribute_dependent_tiles; }
    TileStructure getDefaultTileStructure() const { return default_tile_structure; }
    uint16_t getNumberOfAddedTileStructures() const { return n_add_tile_structures; }
    std::vector<uint16_t> getNumberOfAttributes() const { return n_attributes; }
    std::vector<std::vector<uint16_t>> getAttributeIDs() const { return attribute_ID; }
    std::vector<uint8_t> getNumberOfDescriptors() const { return n_descriptors; }
    std::vector<std::vector<uint8_t>> getDescriptorIDs() const { return descriptor_ID; }
    std::vector<TileStructure> getAdditionalTileStructures() const { return additional_tile_structure; }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace tile_configuration
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_CONTACT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
