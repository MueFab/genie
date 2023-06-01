/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_GENIE_ANNOTATION_CODE_H_
#define SRC_APPS_GENIE_ANNOTATION_CODE_H_

// ---------------------------------------------------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace annotation {

/**
 * @brief
 */
class Code {
 public:
    /**
     * @brief
     */

    Code(std::string inputFileName, std::string OutputFileName, uint8_t encodeMode = 3);

 private:
    using AnnotationParameterSet = genie::core::record::annotation_parameter_set::Record;
    using AnnotationAccessUnit = genie::core::record::annotation_access_unit::Record;
    using DescriptorID = genie::core::record::annotation_parameter_set::DescriptorID;

    uint8_t encodingMode = 3;  // BSC
    const DescriptorID descriptorID = DescriptorID::ATTRIBUTE;
    uint64_t fileSize = 197974;
    uint8_t AT_coord_size = 0;
    uint8_t AG_class = 1;
    uint8_t AT_ID = 1;
    bool variable_size_tiles = false;
    bool attribute_contiguity = false;
    bool two_dimensional = false;
    bool column_major_tile_order = false;

    int bsc_init_done = 0;

    AnnotationParameterSet annotationParameterSet;
    AnnotationAccessUnit annotationAccessUnit;

    void fillAnnotationParameterSet();
    void fillAnntationAccessUnit();
    void encodeData();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_ANNOTATION_CODE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
