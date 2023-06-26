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
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/record/variant_site/record.h"

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
    Code(std::string inputFileName, std::string OutputFileName);
    Code(std::string inputFileName, std::string OutputFileName, bool testOutput);
    Code(std::string inputFileName, std::string OutputFileName, std::string encodeString, bool testOutput);
    Code(std::string inputFileName, std::string OutputFileName, uint8_t encodeMode, bool testOutput);

 private:
    using AnnotationParameterSet = genie::core::record::annotation_parameter_set::Record;
    using AnnotationAccessUnit = genie::core::record::annotation_access_unit::Record;
    using DescriptorID = genie::core::record::annotation_parameter_set::DescriptorID;
    using DescriptorConfiguration = genie::core::record::annotation_parameter_set::DescriptorConfiguration;
    using AnnotationEncodingParameters = genie::core::record::annotation_parameter_set::AnnotationEncodingParameters;

    uint8_t encodingMode = 3;  // BSC
    const DescriptorID descriptorID = DescriptorID::ATTRIBUTE;
    uint64_t fileSize = 197974;
    const uint8_t AT_coord_size = 2;
    const uint8_t AG_class = 1;
    const uint8_t AT_ID = 1;
    bool variable_size_tiles = false;
    bool attribute_contiguity = false;
    bool two_dimensional = false;
    bool column_major_tile_order = false;

    AnnotationParameterSet annotationParameterSet;
    AnnotationAccessUnit annotationAccessUnit;

    std::string inputFileName = "in.mgrecs";
    std::string outputFileName = "out.mgb";
    std::stringstream compressedData{};

    void fillAnnotationParameterSet();
    DescriptorConfiguration fillDescriptorConfiguration();
    AnnotationEncodingParameters fillAnnotationEncodingParameters();

    void fillAnnotationAccessUnit();
    void encodeData();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_ANNOTATION_CODE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
