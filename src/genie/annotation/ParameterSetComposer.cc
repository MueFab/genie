/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>

#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

#include "ParameterSetComposer.h"

// #include "genie/contact/contact_matrix_parameters.h"
#include "genie/core/record/annotation_parameter_set/CompressorParameterSet.h"
#include "genie/core/record/annotation_parameter_set/TileConfiguration.h"
#include "genie/core/record/annotation_parameter_set/TileStructure.h"

#include "genie/genotype/genotype_parameters.h"
#include "genie/likelihood/likelihood_parameters.h"
#include "genie/variantsite/ParameterSetComposer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

genie::core::record::annotation_parameter_set::Record ParameterSetComposer::Compose(
    uint8_t AT_ID, uint8_t AG_class, std::pair<uint64_t, uint64_t> tile_Size,
    core::record::annotation_parameter_set::AnnotationEncodingParameters& annotationEncodingpars) {
    const uint8_t ATCoordSize = 3;
    const bool AT_pos_40_bits_flag = false;
    const core::AlphabetID AT_alphabet_ID = core::AlphabetID::ACGTN;
    const uint8_t n_aux_attribute_groups = 0;
    const uint8_t parameter_set_ID = 1;

    std::vector<genie::core::record::annotation_parameter_set::TileConfiguration> tile_configurations;

    tile_configurations.emplace_back(genie::core::record::annotation_parameter_set::TileConfiguration{
        ATCoordSize, AG_class, {tile_Size.first, tile_Size.second}});

    genie::core::record::annotation_parameter_set::Record annotationParameterSet(
        parameter_set_ID, AT_ID, AT_alphabet_ID, ATCoordSize, AT_pos_40_bits_flag, n_aux_attribute_groups,
        tile_configurations, annotationEncodingpars);
    return annotationParameterSet;
}

}  // namespace annotation
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
