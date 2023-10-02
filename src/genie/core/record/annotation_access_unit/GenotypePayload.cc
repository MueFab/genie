/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include "BlockPayload.h"

#include "genie/core/record/annotation_access_unit/GenotypePayload.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

GenotypePayload::GenotypePayload()
    : num_variants_payloads(0),
      variants_payload_size{},
      variants_payload{},
      sort_variants_row_flag{},
      sort_variants_row_ids_payload_size{},
      sort_variants_row_ids_payload{},
      sort_variants_col_flags{0},
      sort_variants_col_ids_payload_size{},
      sort_variants_col_ids_payload{},
      binarization_id(genie::genotype::BinarizationID::BIT_PLANE),
      variant_amax_payload_size(0),
      encode_phase_data(false),
      phases_payload_size(0),
      phases_payload{} {}

GenotypePayload::GenotypePayload(
    util::BitReader& reader, genie::genotype::GenotypeParameters& genotypeParameters) {
    read(reader, genotypeParameters);
}



}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
