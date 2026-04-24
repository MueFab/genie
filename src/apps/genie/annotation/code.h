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
#include "genie/core/access_unit/annotation/record.h"
#include "genie/core/parameter/annotation/descriptor_configuration.h"
#include "genie/core/parameter/annotation/record.h"
#include "genie/core/record/site/record.h"

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
    Code(const std::string& _inputFileName, const std::string& _outputFileName);
    Code(const std::string& _inputFileName, const std::string& _outputFileName, const std::string& _jsonInfoFileName);
    Code(const std::string& _inputFileName, const std::string& _outputFileName, bool testOutput);
    Code(const std::string& _inputFileName, const std::string& _outputFileName, bool testOutput,
         const std::string& rec = (std::string&)"all");

    Code(const std::string& _inputFileName, const std::string& _outputFileName, bool testOutput,
         const std::string& _infoFieldsFileName, const std::string& rec = (std::string&)"all");

 private:
    using AnnotationParameterSet = genie::core::parameter::annotation::Record;
    using AnnotationAccessUnit = genie::core::access_unit::annotation::Record;
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
