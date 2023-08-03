/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GENOTYPE_PARAMETERS_H
#define GENIE_GENOTYPE_PARAMETERS_H

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <boost/optional/optional.hpp>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/core/constants.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {

// ---------------------------------------------------------------------------------------------------------------------

enum class BinarizationID {
    BIT_PLANE = 0,
    ROW_SPLIT = 1
};

// ---------------------------------------------------------------------------------------------------------------------

enum class ConcatAxis {
    CONCAT_ROW_DIR = 0,
    CONCAT_COL_DIR = 1,
    DO_NOT_CONCAT = 2
};

// ---------------------------------------------------------------------------------------------------------------------

struct GenotypePayloadParameters {
    bool sort_variants_rows_flag;
    bool sort_variants_cols_flag;
    bool transpose_variants_mat_flag;
    genie::core::AlgoID variants_codec_ID;
};

// ---------------------------------------------------------------------------------------------------------------------

class GenotypeParameters {
 private:
    uint8_t max_ploidy;
    bool no_reference_flag;
    bool not_available_flag;
    BinarizationID binarization_ID;

    // if BinarizationID == BinarizationID::BIT_PLANE
    uint8_t num_bit_plane;
    ConcatAxis concat_axis;

    std::vector<GenotypePayloadParameters> variants_payload_params;

    bool encode_phases_data_flag;
    std::vector<GenotypePayloadParameters> phases_payload_params;
    bool phases_value;

 public:
    uint8_t getNumVariantsPayloads() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GENOTYPE_PARAMETERS_H

// ---------------------------------------------------------------------------------------------------------------------
