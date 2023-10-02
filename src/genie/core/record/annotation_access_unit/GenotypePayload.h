/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_GENOTYPEPAYLOAD_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_GENOTYPEPAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/core/writer.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/util/bitreader.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

class GenotypePayload {
 private:
    uint8_t num_variants_payloads;
    std::vector<uint32_t> variants_payload_size;
    std::vector<VariantsPayload> variants_payload;
    std::vector<bool> sort_variants_row_flag;
    std::vector<uint32_t> sort_variants_row_ids_payload_size;
    std::vector<RowColIDsPayload> sort_variants_row_ids_payload;
    std::vector<bool> sort_variants_col_flags;
    std::vector<uint32_t> sort_variants_col_ids_payload_size;
    std::vector<RowColIDsPayload> sort_variants_col_ids_payload;
    genie::genotype::BinarizationID binarization_id;
    uint32_t variant_amax_payload_size;
    AmaxPayload variant_amax_payload;
    bool encode_phase_data;
    uint32_t phases_payload_size;
    BinaryMatrixPayload phases_payload;

    std::vector<genie::genotype::GenotypePayloadParameters> variants_payload_params;
    genie::genotype::GenotypePayloadParameters phases_payload_params;

 public:
    GenotypePayload();
    GenotypePayload(util::BitReader& reader, genie::genotype::GenotypeParameters& genotypeParameters);
    void read(util::BitReader& reader, genie::genotype::GenotypeParameters& genotypeParameters) {
        (void)reader;
        (void)genotypeParameters;
    }
    void read(util::BitReader& reader) { (void)reader; };
    void write(core::Writer& writer) const { (void)writer; }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_GENOTYPEPAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
