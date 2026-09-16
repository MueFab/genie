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
#include "genie/core/access_unit/annotation/access_unit.h"
#include "genie/core/parameter/annotation/descriptor_configuration.h"
#include "genie/core/parameter/annotation/parameter_set.h"
#include "genie/core/record/annotation/site/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace annotation {

/**
 * @brief
 */
class Code {
 public:
    Code() = default;
    Code(const std::string& _inputFileName, const std::string& _outputFileName);
    Code(const std::string& _inputFileName, const std::string& _outputFileName,
         const std::string& _infoFieldsFileName);
    Code(const std::string& _inputFileName, const std::string& _outputFileName, bool testOutput);
    Code(const std::string& _inputFileName, const std::string& _outputFileName, bool testOutput,
         const std::string& rec);
    Code(const std::string& inputFileName, const std::string& outputFileName,
         bool testOutput, const std::string& infoFieldsFileName,
         const std::string& _rec = "");

 private:
    using AnnotationParameterSet = genie::core::parameter::annotation::ParameterSet;
    using AnnotationAccessUnit = genie::core::access_unit::annotation::AccessUnit;
    using DescriptorConfiguration = genie::core::parameter::annotation::DescriptorConfiguration;
    using AnnotationEncodingParameters = genie::core::parameter::annotation::AnnotationEncodingParameters;
};

void encodeVariantSite(const std::string& _inputFileName, const std::string& _outputFileName, bool testOutput,
                       const std::string& _infoFieldsFileName);
void encodeVariantGenotype(const std::string& _input_fpath, const std::string& _output_fpath);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_ANNOTATION_CODE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
