/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_ANNOTATIONACCESSUNITHEADER_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_ANNOTATIONACCESSUNITHEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

/**
 *  @brief
 */
class AnnotationAccessUnitHeader {
 private:
    bool attribute_contiguity;
    bool two_dimensional;
    bool variable_size_tiles;
    bool column_major_tile_order;
    uint8_t AT_coord_size;

    bool is_attribute;
    uint16_t attribute_ID;
    genie::core::record::annotation_parameter_set::DescriptorID descriptor_ID;
    uint64_t n_tiles_per_col;
    uint64_t n_tiles_per_row;
    uint64_t n_blocks;

    uint64_t tile_index_1;
    bool tile_index_2_exists;
    uint64_t tile_index_2;

 public:
    AnnotationAccessUnitHeader();
    explicit AnnotationAccessUnitHeader(util::BitReader& reader);
    AnnotationAccessUnitHeader(util::BitReader& reader, bool attributeContiguity, bool twoDimensional,
                               bool columnMajorTileOrder, bool variable_size_tiles, uint8_t ATCoordSize);
    AnnotationAccessUnitHeader(bool attributeContiguity, bool twoDimensional, bool columnMajorTileOrder,
                               bool variable_size_tiles, uint8_t ATCoordSize, bool is_attribute, uint16_t attribute_ID,
                               genie::core::record::annotation_parameter_set::DescriptorID descriptor_ID,
                               uint64_t n_tiles_per_col, uint64_t n_tiles_per_row, uint64_t n_blocks,
                               uint64_t tile_index_1, bool tile_index_2_exists, uint64_t tile_index_2);

    void read(util::BitReader& reader);
    void read(util::BitReader& reader, bool attributeContiguity, bool twoDimensional, bool columnMajorTileOrder,
              bool variable_size_tiles, uint8_t ATCoordSize);

    void write(core::Writer& writer) const;

    bool ISAttribute() const { return is_attribute; }
    uint16_t getAttributeID() const { return attribute_ID; }
    genie::core::record::annotation_parameter_set::DescriptorID getDescriptorID() const { return descriptor_ID; }
    uint64_t getNumberOfTilesPerColumn() const { return n_tiles_per_col; }
    uint64_t getNumberOfTilesPerRow() const { return n_tiles_per_row; }
    uint64_t getNumberOfBlocks() const { return n_blocks; }

    uint64_t getTileFirstIndex() const { return tile_index_1; }
    bool doesIndex2Exists() const { return tile_index_2_exists; }
    uint64_t getTileSecondIndex() const { return tile_index_2; }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_ANNOTATIONACCESSUNITHEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
