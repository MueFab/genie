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
#include "genie/core/variant_site_record/record.h"

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
    using AnnotationParameterSet = genie::core::record::annotation_parameter_set::Record;
    using AnnotationAccessUnit = genie::core::record::annotation_access_unit::Record;
    using DescriptorConfiguration = genie::core::record::annotation_parameter_set::DescriptorConfiguration;
    using AnnotationEncodingParameters = genie::core::record::annotation_parameter_set::AnnotationEncodingParameters;
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
