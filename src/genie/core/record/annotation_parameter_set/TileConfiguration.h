/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_TILECONFIGURATION_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_TILECONFIGURATION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

#include "TileStructure.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

/**
 *  @brief
 */
class TileConfiguration {
 private:
    uint8_t AT_coord_size;

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
    TileConfiguration();
    explicit TileConfiguration(uint8_t AT_coord_size);
    TileConfiguration(util::BitReader& reader, uint8_t AT_coord_size);
    TileConfiguration(uint8_t AT_coord_size, uint8_t AG_class, uint64_t defaultTileSize);
    TileConfiguration(uint8_t AT_coord_size, uint8_t AG_class, std::vector<uint64_t> defaultTileSize);

    TileConfiguration(uint8_t AT_coord_size, uint8_t AG_class, bool attribute_contiguity, bool two_dimensional,
                      bool column_major_tile_order, uint8_t symmetry_mode, bool symmetry_minor_diagonal,
                      bool attribute_dependent_tiles, TileStructure default_tile_structure,
                      uint16_t n_add_tile_structures, std::vector<uint16_t> n_attributes,
                      std::vector<std::vector<uint16_t>> attribute_ID, std::vector<uint8_t> n_descriptors,
                      std::vector<std::vector<uint8_t>> descriptor_ID,
                      std::vector<TileStructure> additional_tile_structure);

    void read(util::BitReader& reader, uint8_t AT_coord_size);
    void read(util::BitReader& reader);
    void write(core::Writer& writer) const;
    void write(util::BitWriter& writer) const;

    void setAGClass(uint8_t AGClass) { AG_class = AGClass; }
    void setATCoordSize(uint8_t ATCoordSize) { AT_coord_size = ATCoordSize; }

    size_t getSize(core::Writer& writesize) const;

    uint8_t getAttributeGroupClass() const { return AG_class; }
    bool isAttributeContiguity() const { return attribute_contiguity; }
    bool isTwoDimensional() const { return two_dimensional; }
    bool isColumnMajorTileOrder() const { return column_major_tile_order; }
    uint8_t getSymmetryMode() const { return symmetry_mode; }
    bool isSymmetryMinorDiagonal() const { return symmetry_minor_diagonal; }

    bool isAttrributeDependentTiles() const { return attribute_dependent_tiles; }
    TileStructure getDefaultTileStructure() const { return default_tile_structure; }
    uint16_t getNumberOfAddedTileStructures() const { return n_add_tile_structures; }
    std::vector<uint16_t> getNumberOfAttributes() const { return n_attributes; }
    std::vector<std::vector<uint16_t>> getAttributeIDs() const { return attribute_ID; }
    std::vector<uint8_t> getNumberOfDescriptors() const { return n_descriptors; }
    std::vector<std::vector<uint8_t>> getDescriptorIDs() const { return descriptor_ID; }
    std::vector<TileStructure> getAdditionalTileStructures() const { return additional_tile_structure; }
};

struct TileParameterSettings {
    bool attributeContiguity;
    bool twoDimensional;
    bool columnMajorTileOrder;
    bool symmetry_mode;
    bool symmetry_minor_diagonal;
    bool attribute_dependent_tiles;
    uint16_t n_add_tile_structures;

    bool variable_size_tiles;
    TileParameterSettings()
        : attributeContiguity(false),
          twoDimensional(false),
          columnMajorTileOrder(false),
          symmetry_mode(false),
          symmetry_minor_diagonal(false),
          attribute_dependent_tiles(false),
          n_add_tile_structures(0),
          variable_size_tiles(false) {}
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_TILECONFIGURATION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
