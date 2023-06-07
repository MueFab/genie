/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>

#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/util/bitreader.h"

#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

// ---------------------------------------------------------------------------------------------------------------------

Record::Record()
    : AT_ID(0),
      AT_type(AnnotationType::VARIANTS),
      AT_subtype(0),
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

Record::Record(util::BitReader& reader, bool attributeContiguity, bool twoDimensional, bool columnMajorTileOrder,
               uint8_t ATCoordSize, uint8_t numChrs) {
    read(reader, attributeContiguity, twoDimensional, columnMajorTileOrder, ATCoordSize, numChrs);
}

Record::Record(uint8_t AT_ID, AnnotationType AT_type, uint8_t AT_subtype, uint8_t AG_class,
               AnnotationAccessUnitHeader annotation_access_unit_header, std::vector<Block> block,
               bool attributeContiguity, bool twoDimensional, bool columnMajorTileOrder, uint8_t ATCoordSize,
               bool variable_size_tiles, uint64_t n_blocks, uint8_t numChrs)
    : AT_ID(AT_ID),
      AT_type(AT_type),
      AT_subtype(AT_subtype),
      AG_class(AG_class),
      annotation_access_unit_header(annotation_access_unit_header),
      block(block),
      attribute_contiguity(attributeContiguity),
      two_dimensional(twoDimensional),
      column_major_tile_order(columnMajorTileOrder),
      variable_size_tiles(variable_size_tiles),
      n_blocks(n_blocks),
      numChrs(numChrs),
      AT_coord_size(ATCoordSize) {}

void Record::read(util::BitReader& reader) {
    AT_ID = static_cast<uint8_t>(reader.read_b(8));
    AT_type = static_cast<AnnotationType>(reader.read_b(4));
    AT_subtype = static_cast<uint8_t>(reader.read_b(4));
    AG_class = static_cast<uint8_t>(reader.read_b(3));
    reader.read_b(5);
    annotation_access_unit_header.read(reader, attribute_contiguity, two_dimensional, column_major_tile_order,
                                       variable_size_tiles, AT_coord_size);
    n_blocks = annotation_access_unit_header.getNumberOfBlocks();
    block.resize(n_blocks);
    for (auto& blocki : block) blocki.read(reader, numChrs);
}

void Record::read(util::BitReader& reader, bool attributeContiguity, bool twoDimensional, bool columnMajorTileOrder,
                  uint8_t ATCoordSize, uint8_t num_Chrs) {
    attribute_contiguity = attributeContiguity;
    two_dimensional = twoDimensional;
    column_major_tile_order = columnMajorTileOrder;
    AT_coord_size = ATCoordSize;
    numChrs = num_Chrs;
    read(reader);
}

void Record::write(core::Writer& writer) const {
    writer.write(AT_ID, 8);
    writer.write(static_cast<uint8_t>(AT_type), 4);
    writer.write(AT_subtype, 4);
    writer.write(AG_class, 3);
    writer.write(0, 5);
    annotation_access_unit_header.write(writer);
    for (auto& blocki : block) blocki.write(writer);
}

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
