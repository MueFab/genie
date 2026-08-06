/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_RECORD_H_
#define SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

#include "genie/core/access_unit/annotation/annotation_access_unit_header.h"
#include "genie/core/access_unit/annotation/block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::access_unit::annotation {

enum class AnnotationType { VARIANTS = 1, FUNCTIONAL_ANNOTATIONS, GENE_EXPRESSION, CONTACT_MATRICES, TRACKS };

enum class AnnotationSubtype { VCF = 1, GTF = 2, GFF = 3, BED = 4, BEDGRAPH = 5, WIG = 6, BIGWIG = 7, GENBANK = 8, GENE_EXPRESSION = 9, HIC = 10 };

/**
 *  @brief
 */
class Record {
 private:
    uint8_t AT_ID;
    AnnotationType AT_type;
    AnnotationSubtype AT_subtype;
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

    Record(uint8_t AT_ID, AnnotationType AT_type, AnnotationSubtype AT_subtype, uint8_t AG_class,
           AnnotationAccessUnitHeader annotation_access_unit_header, std::vector<Block> block, bool attributeContiguity,
           bool twoDimensional, bool columnMajorTileOrder, uint8_t ATCoordSize, bool variable_size_tiles,
           uint64_t n_blocks, uint8_t numChrs);

    void read(util::BitReader& reader);
    void read(util::BitReader& reader, bool attributeContiguity, bool twoDimensional, bool columnMajorTileOrder,
              uint8_t ATCoordSize, uint8_t numChrs);

    void write(util::BitWriter& writer) const;
    size_t getSize(util::BitWriter& writer) const;
    size_t getSize() const;

    Record& operator=(const Record& rec);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::access_unit::annotation

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
