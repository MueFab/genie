/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_ATTRIBUTE_FIELD_H_
#define SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_ATTRIBUTE_FIELD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::access_unit::annotation {

struct AttributeField {
    std::string name;
    core::DataType type;
    std::vector<std::vector<uint8_t>> values;
};

}  // namespace genie::core::access_unit::annotation

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_ATTRIBUTE_FIELD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------