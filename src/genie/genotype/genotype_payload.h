/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GENOTYPE_PAYLOAD_H
#define GENIE_GENOTYPE_PAYLOAD_H

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

struct VariantsPayload {
    uint32_t variants_payload_size;
    std::vector<uint8_t> variants_payload;
    uint32_t sort_variants_row_ids_payload_size;
};

// ---------------------------------------------------------------------------------------------------------------------

struct PhasesPaylod {
    bool sort_variants_rows_flag;
    bool sort_variants_cols_flag;
    bool transpose_variants_mat_flag;
    genie::core::AlgoID variants_codec_ID;
};

// ---------------------------------------------------------------------------------------------------------------------

class GenotypePayload {
 private:
 public:
//    std::vector<std::vector<uint8_t>>
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GENOTYPE_PAYLOAD_H

// ---------------------------------------------------------------------------------------------------------------------
