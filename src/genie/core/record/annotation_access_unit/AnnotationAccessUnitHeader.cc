/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "AnnotationAccessUnitHeader.h"
#include <algorithm>
#include <string>
#include <utility>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

AnnotationAccessUnitHeader::AnnotationAccessUnitHeader()
    : attribute_contiguity(false),
      two_dimensional(false),
      column_major_tile_order(false),
      variable_size_tiles(false),
      AT_coord_size(0),
      is_attribute(false),
      attribute_ID(0),
      descriptor_ID(AnnotDesc::ATTRIBUTE),
      n_tiles_per_col(0),
      n_tiles_per_row(0),
      n_blocks(0),
      tile_index_1(0),
      tile_index_2_exists(false),
      tile_index_2(0) {}

AnnotationAccessUnitHeader::AnnotationAccessUnitHeader(util::BitReader& reader) { read(reader); }
AnnotationAccessUnitHeader::AnnotationAccessUnitHeader(util::BitReader& reader, bool attributeContiguity,
                                                       bool twoDimensional, bool columnMajorTileOrder,
                                                       bool variable_size_tiles, uint8_t ATCoordSize) {
    read(reader, attributeContiguity, twoDimensional, columnMajorTileOrder, variable_size_tiles, ATCoordSize);
}
AnnotationAccessUnitHeader::AnnotationAccessUnitHeader(
    bool _attributeContiguity,
    bool _twoDimensional,
    bool _columnMajorTileOrder,
    bool _variable_size_tiles,
    uint8_t ATCoordSize,
    bool _is_attribute,
    uint16_t _attribute_ID,
    AnnotDesc _descriptor_ID,
    uint64_t _n_tiles_per_col,
    uint64_t _n_tiles_per_row,
    uint64_t _nblocks,
    uint64_t _tile_index_1,
    bool _tile_index_2_exists,
    uint64_t _tile_index_2)
    : attribute_contiguity(_attributeContiguity),
      two_dimensional(_twoDimensional),
      variable_size_tiles(_variable_size_tiles),
      column_major_tile_order(_columnMajorTileOrder),
      AT_coord_size(ATCoordSize),
      is_attribute(_is_attribute),
      attribute_ID(_attribute_ID),
      descriptor_ID(_descriptor_ID),
      n_tiles_per_col(_n_tiles_per_col),
      n_tiles_per_row(_n_tiles_per_row),
      n_blocks(_nblocks),
      tile_index_1(_tile_index_1),
      tile_index_2_exists(_tile_index_2_exists),
      tile_index_2(_tile_index_2) {}

void AnnotationAccessUnitHeader::read(util::BitReader& reader, bool attributeContiguity, bool twoDimensional,
                                      bool columnMajorTileOrder, bool variableSizeTiles, uint8_t ATCoordSize) {
    attribute_contiguity = attributeContiguity;
    two_dimensional = twoDimensional;
    column_major_tile_order = columnMajorTileOrder;
    AT_coord_size = ATCoordSize;
    variable_size_tiles = variableSizeTiles;
    read(reader);
}

void AnnotationAccessUnitHeader::write(core::Writer& writer) const {
    uint8_t ATCoordBits = 8 << static_cast<uint8_t>(AT_coord_size); 
    if (attribute_contiguity) {
        writer.write(is_attribute, 1);
        if (is_attribute)
            writer.write(attribute_ID, 16);
        else
            writer.write(static_cast<uint8_t>(descriptor_ID), 7);
        if (two_dimensional && !variable_size_tiles) {
            if (column_major_tile_order)
                writer.write(n_tiles_per_col, ATCoordBits);
            else
                writer.write(n_tiles_per_row, ATCoordBits);
        }
        writer.write(n_blocks, ATCoordBits);
    } else {
        writer.write(tile_index_1, ATCoordBits);
        writer.write(tile_index_2_exists, 1);
        if (tile_index_2_exists) writer.write(tile_index_2, ATCoordBits);
        writer.write(n_blocks, 16);
    }
    writer.flush();
}

size_t AnnotationAccessUnitHeader::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.getBitsWritten();
}

void AnnotationAccessUnitHeader::read(util::BitReader& reader) {
    uint8_t ATCoordBits = 8 << static_cast<uint8_t>(AT_coord_size); 
    if (attribute_contiguity) {
        is_attribute = static_cast<bool>(reader.read_b(1));
        if (is_attribute) {
            attribute_ID = static_cast<uint16_t>(reader.read_b(16));
        } else {
            descriptor_ID = static_cast<AnnotDesc>(reader.read_b(7));
        }
        if (two_dimensional && !variable_size_tiles) {
            if (column_major_tile_order) {
                n_tiles_per_col = static_cast<uint64_t>(reader.read_b(ATCoordBits));
            } else {
                n_tiles_per_row = static_cast<uint64_t>(reader.read_b(ATCoordBits));
            }
            n_blocks = static_cast<uint64_t>(reader.read_b(ATCoordBits));
        }
    } else {
        tile_index_1 = static_cast<uint64_t>(reader.read_b(ATCoordBits));
        tile_index_2_exists = static_cast<bool>(reader.read_b(1));
        if (tile_index_2_exists) tile_index_2 = static_cast<uint64_t>(reader.read_b(ATCoordBits));
        n_blocks = static_cast<uint64_t>(reader.read_b(16));
    }
    reader.flush();
}

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
