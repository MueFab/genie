/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_RECORD_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bit_reader.h"

#include "AnnotationAccessUnitHeader.h"
#include "genie/core/record/annotation_access_unit/block.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

enum class AnnotationType { VARIANTS = 1, FUNCTIONAL_ANNOTATIONS, GENIE_EXPRESSION, CONTACT_MATRICES, TRACKS };

/**
 *  @brief
 */
class Record {
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
     * @brief
     */
    Record();
    explicit Record(util::BitReader& reader);
    Record(util::BitReader& reader, bool attributeContiguity, bool twoDimensional, bool columnMajorTileOrder,
           uint8_t ATCoordSize, uint8_t numChrs);

    Record(uint8_t AT_ID, AnnotationType AT_type, uint8_t AT_subtype, uint8_t AG_class,
           AnnotationAccessUnitHeader annotation_access_unit_header, std::vector<Block> block, bool attributeContiguity,
           bool twoDimensional, bool columnMajorTileOrder, uint8_t ATCoordSize, bool variable_size_tiles,
           uint64_t n_blocks, uint8_t numChrs);

    void read(util::BitReader& reader);
    void read(util::BitReader& reader, bool attributeContiguity, bool twoDimensional, bool columnMajorTileOrder,
              uint8_t ATCoordSize, uint8_t numChrs);

    void write(core::Writer& writer) const;
    size_t getSize() const;
    size_t getSize(core::Writer& writesize) const;

    Record& operator=(const Record& rec);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
