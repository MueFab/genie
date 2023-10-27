/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_GENOTYPE_PARAMETERSETCOMPOSER_H_
#define SRC_GENIE_GENOTYPE_PARAMETERSETCOMPOSER_H_

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
#include "genie/util/bitreader.h"

#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/jbig/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"

#include "genie/genotype/genotype_coder.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/genotype/genotype_payload.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {

class ParameterSetComposer {
 public:
    genie::core::record::annotation_parameter_set::Record Build(
        uint8_t _AT_ID, std::map<std::string, genie::core::record::variant_site::AttributeData>& info, genie::genotype::GenotypeParameters& genotypeParameters, uint64_t defaultTileSize = 100);


};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_GENOTYPE_PARAMETERSETCOMPOSER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
