/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_ANNOTATION_ACCESS_UNIT_ANNOTATION_ACCESS_UNIT_HEADER_H
#define SRC_GENIE_FORMAT_MGG_ANNOTATION_ACCESS_UNIT_ANNOTATION_ACCESS_UNIT_HEADER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>

#include "genie/core/constants.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit_reader.h"

#define GENIE_DEBUG_PRINT_NODETAIL

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace annotation_access_unit {

class AnnotationAccessUnitHeader : public GenInfo {
 public:
    AnnotationAccessUnitHeader(
        bool _is_transport_mode,
        uint8_t _dataset_group_ID,
        uint16_t _dataset_ID,
        uint8_t _AT_ID,
        uint8_t _AG_class,
        bool _attribute_contiguity,
        bool _two_dimensional,
        bool _column_major_tile_order,
        bool _variable_size_tiles,
//        uint8_t _ATCoordSize,
        bool _is_attribute,
        uint16_t _attribute_ID,
        genie::core::AnnotDesc _descriptor_ID,
        uint64_t _n_tiles_per_col,
        uint64_t _n_tiles_per_row,
        uint64_t _n_blocks,
        uint64_t _tile_index_1,
        bool _tile_index_2_exists,
        uint64_t _tile_index_2
    );

 private:
    bool is_transport_mode;
    // IF is_transport_mode == 1
    uint8_t dataset_group_ID;
    uint16_t dataset_ID;
    uint8_t AT_ID;
    uint8_t AG_class;

    bool attribute_contiguity;
    // IF attribute_contiguity == 1
    bool is_attribute;
    uint16_t attribute_ID;
    genie::core::AnnotDesc descriptor_ID;
    bool two_dimensional; // From other structure
    bool variable_size_tiles; // From other structure
    bool column_major_tile_order;  // From other structure
    uint64_t n_tiles_per_col;
    uint64_t n_tiles_per_row;
    uint64_t n_blocks; // n_AAU_blocks

    uint64_t tile_index_1;
    bool tile_index_2_exists;
    uint64_t tile_index_2;

};


// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_access_unit
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_ANNOTATION_ACCESS_UNIT_ANNOTATION_ACCESS_UNIT_HEADER_H
