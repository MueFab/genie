/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/access_unit/annotation/access_unit.h"
#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include "genie/util/bit_reader.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::access_unit::annotation {

AccessUnit::AccessUnit()
    : AT_ID(0),
      AT_type(AnnotationType::VARIANTS),
      AT_subtype(1),
      AG_class(0),
      annotation_access_unit_header{},
      block{},
      attribute_contiguity(false),
      two_dimensional(false),
      column_major_tile_order(false),
      variable_size_tiles(false),
      AT_coord_size(0),
      n_blocks(0),
      numChrs(0) {}

AccessUnit::AccessUnit(util::BitReader& reader, bool attributeContiguity, bool twoDimensional, bool columnMajorTileOrder,
                       uint8_t ATCoordSize, uint8_t numChrs) {
    read(reader, attributeContiguity, twoDimensional, columnMajorTileOrder, ATCoordSize, numChrs);
}

AccessUnit::AccessUnit(uint8_t AT_ID, AnnotationType AT_type, uint8_t AT_subtype, uint8_t AG_class,
                       AnnotationAccessUnitHeader annotation_access_unit_header, std::vector<Block> blocks,
                       bool attributeContiguity, bool twoDimensional, bool columnMajorTileOrder, uint8_t ATCoordSize,
                       bool variable_size_tiles, uint64_t n_blocks, uint8_t numChrs)
    : AT_ID(AT_ID),
      AT_type(AT_type),
      AT_subtype(AT_subtype),
      AG_class(AG_class),
      annotation_access_unit_header(annotation_access_unit_header),
      attribute_contiguity(attributeContiguity),
      two_dimensional(twoDimensional),
      column_major_tile_order(columnMajorTileOrder),
      variable_size_tiles(variable_size_tiles),
      AT_coord_size(ATCoordSize),
      n_blocks(n_blocks),
      numChrs(numChrs) {
    this->block.resize(n_blocks);
    for (size_t idx_i = 0; idx_i < blocks.size(); ++idx_i) {
        this->block[idx_i] = blocks[idx_i];
    }
}

void AccessUnit::read(util::BitReader& reader) {
    AT_ID = static_cast<uint8_t>(reader.ReadBits(8));
    AT_type = static_cast<AnnotationType>(reader.ReadBits(4));
    AT_subtype = static_cast<uint8_t>(reader.ReadBits(4));
    AG_class = static_cast<uint8_t>(reader.ReadBits(3));
    reader.ReadBits(5);
    annotation_access_unit_header.read(reader, attribute_contiguity, two_dimensional, column_major_tile_order,
                                       variable_size_tiles, AT_coord_size);
    n_blocks = annotation_access_unit_header.getNumberOfBlocks();
    block.resize(n_blocks);
    for (auto& blocki : block) {
        blocki.read(reader, numChrs);
    }
}

void AccessUnit::read(util::BitReader& reader, bool attributeContiguity, bool twoDimensional, bool columnMajorTileOrder,
                      uint8_t ATCoordSize, uint8_t num_Chrs) {
    attribute_contiguity = attributeContiguity;
    two_dimensional = twoDimensional;
    column_major_tile_order = columnMajorTileOrder;
    AT_coord_size = ATCoordSize;
    numChrs = num_Chrs;
    read(reader);
}

void AccessUnit::write(util::BitWriter& writer) const {
    writer.WriteBits(AT_ID, 8);
    writer.WriteBits(static_cast<uint8_t>(AT_type), 4);
    writer.WriteBits(AT_subtype, 4);
    writer.WriteBits(AG_class, 3);
    writer.WriteBits(0, 5);
    annotation_access_unit_header.write(writer);
    for (auto& blocki : block) {
        blocki.write(writer);
    }
}

size_t AccessUnit::getSize(util::BitWriter& writer) const {
    write(writer);
    return writer.GetTotalBitsWritten();
}

size_t AccessUnit::getSize() const {
    return 0;
}

AccessUnit& AccessUnit::operator=(const AccessUnit& rec) {
    if (this == &rec) return *this;
    AT_ID = rec.AT_ID;
    AT_type = rec.AT_type;
    AT_subtype = rec.AT_subtype;
    AG_class = rec.AG_class;
    annotation_access_unit_header = rec.annotation_access_unit_header;
    attribute_contiguity = rec.attribute_contiguity;
    two_dimensional = rec.two_dimensional;
    column_major_tile_order = rec.column_major_tile_order;
    variable_size_tiles = rec.variable_size_tiles;
    AT_coord_size = rec.AT_coord_size;
    n_blocks = rec.n_blocks;
    numChrs = rec.numChrs;
    block.clear();
    for (const auto& blockin : rec.block) {
        block.push_back(blockin);
    }
    return *this;
}

}  // namespace genie::core::access_unit::annotation
