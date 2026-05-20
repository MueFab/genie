/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/annotation_access_unit/record.h"

#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

// ---------------------------------------------------------------------------------------------------------------------

Record::Record()
    : AT_ID(0),
      AT_type(AnnotationType::VARIANTS),
      AT_subtype(AnnotationSubtype::VCF),
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
    Read(reader, attributeContiguity, twoDimensional, columnMajorTileOrder, ATCoordSize, numChrs);
}

Record::Record(uint8_t AT_ID, AnnotationType AT_type, AnnotationSubtype AT_subtype, uint8_t AG_class,
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
    for (size_t i = 0; i < blocks.size(); ++i) this->block[i] = blocks[i];
}

void Record::Read(util::BitReader& reader) {
    AT_ID = static_cast<uint8_t>(reader.ReadBits(8));
    AT_type = static_cast<AnnotationType>(reader.ReadBits(4));
    AT_subtype = static_cast<AnnotationSubtype>(reader.ReadBits(4));
    AG_class = static_cast<uint8_t>(reader.ReadBits(3));
    reader.ReadBits(5);
    annotation_access_unit_header.Read(reader, attribute_contiguity, two_dimensional, column_major_tile_order,
                                       variable_size_tiles, AT_coord_size);
    n_blocks = annotation_access_unit_header.getNumberOfBlocks();
    block.resize(n_blocks);
    for (auto& blocki : block) blocki.Read(reader, numChrs);
}

void Record::Read(util::BitReader& reader, bool attributeContiguity, bool twoDimensional, bool columnMajorTileOrder,
                  uint8_t ATCoordSize, uint8_t num_Chrs) {
    attribute_contiguity = attributeContiguity;
    two_dimensional = twoDimensional;
    column_major_tile_order = columnMajorTileOrder;
    AT_coord_size = ATCoordSize;
    numChrs = num_Chrs;
    Read(reader);
}

void Record::Write(util::BitWriter& writer) const {
    writer.WriteBits(AT_ID, 8);
    writer.WriteBits(static_cast<uint8_t>(AT_type), 4);
    writer.WriteBits(static_cast<uint8_t>(AT_subtype), 4);
    writer.WriteBits(AG_class, 3);
    writer.WriteReserved(5);
    annotation_access_unit_header.Write(writer);
    for (auto& blocki : block) blocki.Write(writer);
}

size_t Record::GetSize() const {
    std::stringstream SizeOut;
    util::BitWriter writesize(&SizeOut);
    return GetSize(writesize);
}

size_t Record::GetSize(util::BitWriter& writesize) const {
    Write(writesize);
    return writesize.GetTotalBitsWritten();
}

Record& Record::operator=(const Record& rec) {
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
    for (const auto& blockin : rec.block) block.push_back(blockin);
    return *this;
}

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
