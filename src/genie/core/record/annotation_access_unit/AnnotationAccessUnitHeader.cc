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
      descriptor_ID(genie::core::record::annotation_parameter_set::DescriptorID::ATTRIBUTE),
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
    bool attributeContiguity, bool twoDimensional, bool columnMajorTileOrder, bool variable_size_tiles,
    uint8_t ATCoordSize, bool is_attribute, uint16_t attribute_ID,
    genie::core::record::annotation_parameter_set::DescriptorID descriptor_ID, uint64_t n_tiles_per_col,
    uint64_t n_tiles_per_row, uint64_t n_blocks, uint64_t tile_index_1, bool tile_index_2_exists, uint64_t tile_index_2)
    : attribute_contiguity(attributeContiguity),
      two_dimensional(twoDimensional),
      column_major_tile_order(columnMajorTileOrder),
      variable_size_tiles(variable_size_tiles),
      AT_coord_size(ATCoordSize),
      is_attribute(is_attribute),
      attribute_ID(attribute_ID),
      descriptor_ID(descriptor_ID),
      n_tiles_per_col(n_tiles_per_col),
      n_tiles_per_row(n_tiles_per_row),
      n_blocks(n_blocks),
      tile_index_1(tile_index_1),
      tile_index_2_exists(tile_index_2_exists),
      tile_index_2(tile_index_2) {}

void AnnotationAccessUnitHeader::read(util::BitReader& reader, bool attributeContiguity, bool twoDimensional,
                                      bool columnMajorTileOrder, bool variableSizeTiles, uint8_t ATCoordSize) {
    attribute_contiguity = attributeContiguity;
    two_dimensional = twoDimensional;
    column_major_tile_order = columnMajorTileOrder;
    AT_coord_size = ATCoordSize;
    variable_size_tiles = variableSizeTiles;
    read(reader);
}

void AnnotationAccessUnitHeader::write(util::BitWriter& writer) const {
    if (attribute_contiguity) {
        writer.write(is_attribute, 1);
        if (is_attribute)
            writer.write(attribute_ID, 16);
        else
            writer.write(static_cast<uint8_t>(descriptor_ID), 7);
        if (two_dimensional && !variable_size_tiles) {
            if (column_major_tile_order)
                writer.write(n_tiles_per_col, (AT_coord_size + 1) * 8);
            else
                writer.write(n_tiles_per_row, (AT_coord_size + 1) * 8);
            writer.write(n_blocks, (AT_coord_size + 1) * 8);
        }
    } else {
        writer.write(tile_index_1, (AT_coord_size + 1) * 8);
        writer.write(tile_index_2_exists, 1);
        if (tile_index_2_exists) writer.write(tile_index_2, (AT_coord_size + 1) * 8);
        writer.write(n_blocks, 16);
    }
    writer.flush();
}

void AnnotationAccessUnitHeader::write(std::ostream& outputfile) const { outputfile << "not implemented\n"; }

void AnnotationAccessUnitHeader::read(util::BitReader& reader) {
    if (attribute_contiguity) {
        is_attribute = static_cast<bool>(reader.read_b(1));
        if (is_attribute) {
            attribute_ID = static_cast<uint16_t>(reader.read_b(16));
        } else {
            descriptor_ID = static_cast<genie::core::record::annotation_parameter_set::DescriptorID>(reader.read_b(7));
        }
        if (two_dimensional && !variable_size_tiles) {
            if (column_major_tile_order) {
                n_tiles_per_col = static_cast<uint64_t>(reader.read_b(8 * (AT_coord_size + 1)));
            } else {
                n_tiles_per_row = static_cast<uint64_t>(reader.read_b(8 * (AT_coord_size + 1)));
            }
            n_blocks = static_cast<uint64_t>(reader.read_b(8 * (AT_coord_size + 1)));
        }
    } else {
        tile_index_1 = static_cast<uint64_t>(reader.read_b(8 * (AT_coord_size + 1)));
        tile_index_2_exists = static_cast<bool>(reader.read_b(1));
        if (tile_index_2_exists) tile_index_2 = static_cast<uint64_t>(reader.read_b(8 * (AT_coord_size + 1)));
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
