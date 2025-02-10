/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ANNOTATION_PARAMETERSET_COMPOSER_H_
#define SRC_GENIE_ANNOTATION_PARAMETERSET_COMPOSER_H_

// ---------------------------------------------------------------------------------------------------------------------
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/arrayType.h"
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bit_reader.h"

#include "genie/annotation/compressors.h"
#include "genie/core/record/annotation_parameter_set/AnnotationEncodingParameters.h"
#include "genie/core/record/annotation_parameter_set/AttributeData.h"
#include "genie/core/record/annotation_parameter_set/record.h"

#include "genie/contact/contact_matrix_parameters.h"
#include "genie/likelihood/likelihood_parameters.h"
#include "genie/genotype/genotype_parameters.h"


// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

class ParameterSetComposer {
 public:
    genie::core::record::annotation_parameter_set::Record Compose(
        uint8_t AT_ID, uint8_t AG_class, std::pair<uint64_t, uint64_t> tile_Size,
                         core::record::annotation_parameter_set::AnnotationEncodingParameters& annotationEncodingpars);


 private:
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_PARAMETERSET_COMPOSER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
