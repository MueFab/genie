/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include "genie/format/mgg/annotation_access_unit/annotation_access_unit_header.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace annotation_access_unit {

AnnotationAccessUnitHeader::AnnotationAccessUnitHeader(
    bool _is_tranport_mode,
    uint8_t _dataset_group_ID,
    uint16_t _dataset_ID,
    uint8_t _AT_ID,
    uint8_t _AG_class,
    bool _attribute_contiguity,
    bool _two_dimensional,
    bool _column_major_tile_order,
    bool _variable_size_tiles,
//    uint8_t _ATCoordSize,
    bool _is_attribute,
    uint16_t _attribute_ID,
    genie::core::AnnotDesc _descriptor_ID,
    uint64_t _n_tiles_per_col,
    uint64_t _n_tiles_per_row,
    uint64_t _n_blocks,
    uint64_t _tile_index_1,
    bool _tile_index_2_exists,
    uint64_t _tile_index_2
    ):
    is_transport_mode(_is_tranport_mode),
    dataset_group_ID(_dataset_group_ID),
    dataset_ID(_dataset_ID),
    AT_ID(_AT_ID),
    AG_class(_AG_class),
    attribute_contiguity(_attribute_contiguity),
    is_attribute(_is_attribute),
    attribute_ID(_attribute_ID),
    descriptor_ID(_descriptor_ID),
//    ATCoordSize(_ATCoordSize),
    two_dimensional(_two_dimensional),
    variable_size_tiles(_variable_size_tiles),
    column_major_tile_order(_column_major_tile_order),
    n_tiles_per_col(_n_tiles_per_col),
    n_tiles_per_row(_n_tiles_per_row),
    n_blocks(_n_blocks),
    tile_index_1(_tile_index_1),
    tile_index_2_exists(_tile_index_2_exists),
    tile_index_2(_tile_index_2)
{}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_access_unit
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
