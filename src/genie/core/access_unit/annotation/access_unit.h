/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_ACCESS_UNIT_H_
#define SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/access_unit/annotation/annotation_access_unit_header.h"
#include "genie/core/access_unit/annotation/block.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::access_unit::annotation {

enum class AnnotationType { VARIANTS = 1, FUNCTIONAL_ANNOTATIONS, GENIE_EXPRESSION, CONTACT_MATRICES, TRACKS };

/**
 * @brief ISO/IEC 23092-6 Annotation Access Unit
 */
class AccessUnit {
 private:
    uint8_t AT_ID;
    AnnotationType AT_type;
    uint8_t AT_subtype;
    uint8_t AG_class;
    AnnotationAccessUnitHeader annotation_access_unit_header;
    std::vector<Block> block;

    bool attribute_contiguity;
    bool two_dimensional;
    bool column_major_tile_order;
    bool variable_size_tiles;
    uint8_t AT_coord_size;
    uint64_t n_blocks;
    uint8_t numChrs;

 public:
    /**
     * @brief Default constructor
     */
    AccessUnit();
    explicit AccessUnit(util::BitReader& reader);
    AccessUnit(util::BitReader& reader, bool attributeContiguity, bool twoDimensional, bool columnMajorTileOrder,
               uint8_t ATCoordSize, uint8_t numChrs);

    AccessUnit(uint8_t AT_ID, AnnotationType AT_type, uint8_t AT_subtype, uint8_t AG_class,
               AnnotationAccessUnitHeader annotation_access_unit_header, std::vector<Block> block, bool attributeContiguity,
               bool twoDimensional, bool columnMajorTileOrder, uint8_t ATCoordSize, bool variable_size_tiles,
               uint64_t n_blocks, uint8_t numChrs);

    void read(util::BitReader& reader);
    void read(util::BitReader& reader, bool attributeContiguity, bool twoDimensional, bool columnMajorTileOrder,
              uint8_t ATCoordSize, uint8_t numChrs);

    void write(util::BitWriter& writer) const;
    [[nodiscard]] size_t getSize(util::BitWriter& writer) const;
    [[nodiscard]] size_t getSize() const;

    AccessUnit& operator=(const AccessUnit& rec);

    [[nodiscard]] uint8_t getATID() const { return AT_ID; }
    [[nodiscard]] AnnotationType getATType() const { return AT_type; }
    [[nodiscard]] uint8_t getATSubtype() const { return AT_subtype; }
    [[nodiscard]] uint8_t getAGClass() const { return AG_class; }
    [[nodiscard]] const AnnotationAccessUnitHeader& getHeader() const { return annotation_access_unit_header; }
    [[nodiscard]] AnnotationAccessUnitHeader& getHeader() { return annotation_access_unit_header; }
    [[nodiscard]] const std::vector<Block>& getBlocks() const { return block; }
    [[nodiscard]] std::vector<Block>& getBlocks() { return block; }
};

}  // namespace genie::core::access_unit::annotation

#endif  // SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_ACCESS_UNIT_H_
