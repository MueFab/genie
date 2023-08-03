/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_ANNOTATION_TABLE_ANNOTATION_TABLE_HEADER_H
#define SRC_GENIE_FORMAT_MGG_ANNOTATION_TABLE_ANNOTATION_TABLE_HEADER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>

#include "genie/format/mgg/gen_info.h"

#define GENIE_DEBUG_PRINT_NODETAIL

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace annotation_table {

class AnnotationTableHeader : public GenInfo {
 public:

 private:
    // IF is_transport_mode == 1
    uint8_t dataset_group_ID;
    uint8_t dataset_ID;

    uint8_t AT_ID;
    uint8_t AT_type;
    uint8_t AT_subtype;
    uint8_t AT_subtype_major_version;
    uint8_t AT_subtype_minor_version;
    std::string AT_name;
    uint8_t AT_version;
    uint8_t AT_coord_size;
    uint8_t AT_genomic_pos_sort_mode;
    bool AT_pos_40_bits_flag;
    uint8_t parameter_set_ID;
    uint8_t n_aux_attribute_groups;
    bool AG_info_exists;

    // IF AG_info_exists == 1
    std::vector<uint8_t> AG_class;
    std::vector<std::string> AG_group_name;
    std::vector<uint16_t> n_fields_AG;
    std::vector<std::vector<bool>> is_attribute;
    std::vector<std::vector<uint16_t>> field_ID;
};

} // namespace annotation_table
} // namespace mgg
} // namespace format
} // namespace genie

#endif  // SRC_GENIE_FORMAT_MGG_ANNOTATION_TABLE_ANNOTATION_TABLE_HEADER_H
