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

#include "genie/core/constants.h"
#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/record/variant_site/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace annotation {

class TempEncoder {
 public:
    void encode(const std::stringstream& input, std::stringstream& output);
};

/**
 * @brief
 */

class Code {
 public:
    /**
     * @brief
     */
    Code(const std::string& _inputFileName, const std::string& _outputFileName);
    Code(const std::string& _inputFileName, const std::string& _outputFileName, bool testOutput);
    Code(const std::string& _inputFileName, const std::string& _outputFileName, std::string encodeString,
         bool testOutput);
    Code(const std::string& _inputFileName, const std::string& _outputFileName, genie::core::AlgoID encodeMode,
         bool testOutput, std::string& rec = (std::string&)"all");
    Code(const std::string& _inputFileName, const std::string& _outputFileName, genie::core::AlgoID encodeMode,
         bool testOutput, const std::string& _infoFieldsFileName, const std::string& _configFileName,
         std::string& rec = (std::string&)"all");

 private:
    using AnnotationParameterSet = genie::core::record::annotation_parameter_set::Record;
    using AnnotationAccessUnit = genie::core::record::annotation_access_unit::Record;
    using DescriptorConfiguration = genie::core::record::annotation_parameter_set::DescriptorConfiguration;
    using AnnotationEncodingParameters = genie::core::record::annotation_parameter_set::AnnotationEncodingParameters;

    genie::core::AlgoID encodingMode = genie::core::AlgoID::BSC;
    const genie::core::AnnotDesc descriptorID = genie::core::AnnotDesc::ATTRIBUTE;
    uint64_t fileSize = 197974;  // TODO @Stefanie: What is the meaning of this number?
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
    std::string infoFieldsFileName = "";
    std::string configFileName = "";
    std::stringstream compressedData{};

    genie::core::AlgoID convertStringToALgoID(std::string algoString) const;

    void fillAnnotationParameterSet();
    DescriptorConfiguration fillDescriptorConfiguration();
    AnnotationEncodingParameters fillAnnotationEncodingParameters();

    void fillAnnotationAccessUnit();
    void encodeData();
};

void encodeVariantGenotype(std::string& _input_fpath, std::string& _output_fpath);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_ANNOTATION_CODE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
