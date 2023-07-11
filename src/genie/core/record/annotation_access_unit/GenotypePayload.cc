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
      sort_variants_col_flags{0},
      sort_variants_row_flag{},
      sort_variants_row_ids_payload_size{},
      sort_variants_col_ids_payload_size{},
      sort_variants_col_ids_payload{},
      sort_variants_row_ids_payload{},
      binarization_id(genie::core::record::annotation_parameter_set::BinarizationID::BIT_PLANE),
      variant_amax_payload_size(0),
      encode_phase_data(false),
      phases_payload_size(0),
      phases_payload{} {}

GenotypePayload::GenotypePayload(
    util::BitReader& reader, genie::core::record::annotation_parameter_set::GenotypeParameters& genotypeParameters) {
    read(reader, genotypeParameters);
}

void GenotypePayload::read(util::BitReader& reader,
                           genie::core::record::annotation_parameter_set::GenotypeParameters& genotypeParameters) {
    binarization_id = genotypeParameters.getBinarizationID();
    num_variants_payloads = 1;
    if (binarization_id == genie::core::record::annotation_parameter_set::BinarizationID::BIT_PLANE &&
        genotypeParameters.getConcatAxis() == genie::core::record::annotation_parameter_set::ConcatAxis::DO_NOT_CONCAT)
        num_variants_payloads = genotypeParameters.getNumberOfBitPlane();
    sort_variants_col_flags = genotypeParameters.getSortVariantsColsFlags();
    sort_variants_row_flag = genotypeParameters.getSortVariantsRowsFlags();
    encode_phase_data = genotypeParameters.isEncodePhaseData();
    read(reader);
}

void GenotypePayload::read(util::BitReader& reader) {
    for (auto i = 0; i < num_variants_payloads; ++i) {
        variants_payload_size.push_back(static_cast<uint32_t>(reader.read_b(32)));
        VariantsPayload variant_payload;
        variant_payload.read(reader);
        variants_payload.push_back(variant_payload);
        if (sort_variants_row_flag[i]) {
            sort_variants_row_ids_payload_size.push_back(static_cast<uint32_t>(reader.read_b(32)));
            RowColIDsPayload row_id_payload;
            row_id_payload.read(reader);
            sort_variants_row_ids_payload.push_back(row_id_payload);
        }
        if (sort_variants_col_flags[i]) {
            sort_variants_col_ids_payload_size.push_back(static_cast<uint32_t>(reader.read_b(32)));
            RowColIDsPayload col_id_payload;
            col_id_payload.read(reader);
            sort_variants_col_ids_payload.push_back(col_id_payload);
        }
    }
    if (binarization_id == genie::core::record::annotation_parameter_set::BinarizationID::ROW_SPLIT) {
        variant_amax_payload_size = static_cast<uint32_t>(reader.read_b(32));
        variant_amax_payload.read(reader);
    }
    if (encode_phase_data) {
        phases_payload_size = static_cast<uint32_t>(reader.read_b(32));
        phases_payload.read(reader);
    }
}

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
