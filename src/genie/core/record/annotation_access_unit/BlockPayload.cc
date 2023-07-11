/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "BlockPayload.h"
#include <algorithm>
#include <sstream>
#include <string>
#include <utility>

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

RowColIDsPayload::RowColIDsPayload() : nbits_per_elem(0), nelements(0), row_col_ids_elements{} {}


BlockPayload::BlockPayload()
    : descriptor_ID(genie::core::record::annotation_parameter_set::DescriptorID::GENOTYPE),
      num_chrs(0),
      genotype_payload{},
      likelihood_payload{},
      cm_bin_payload{},
      cm_mat_payload{},
      block_payload_size(0),
      generic_payload{} {}

BlockPayload::BlockPayload(util::BitReader& reader,
                                       genie::core::record::annotation_parameter_set::DescriptorID descriptorID,
                                       uint8_t numChrs) {
    descriptor_ID = descriptorID;
    num_chrs = numChrs;
    read(reader);
}

BlockPayload::BlockPayload(genie::core::record::annotation_parameter_set::DescriptorID descriptorID,
                                       uint8_t numChrs, GenotypePayload genotype_payload,
                                       LikelihoodPayload likelihood_payload,
                                       std::vector<ContactMatrixBinPayload> cm_bin_payload,
                                       ContactMatrixMatPayload cm_mat_payload, uint32_t block_payload_size,
                                       const std::vector<uint8_t>& genericPayload)
    : descriptor_ID(descriptorID),
      num_chrs(numChrs),
      genotype_payload(genotype_payload),
      likelihood_payload(likelihood_payload),
      cm_bin_payload(cm_bin_payload),
      cm_mat_payload(cm_mat_payload),
      block_payload_size(block_payload_size),
      generic_payload(genericPayload) {}

void BlockPayload::read(util::BitReader& reader) {
    if (descriptor_ID == genie::core::record::annotation_parameter_set::DescriptorID::GENOTYPE) {
        genotype_payload.read(reader);
    } else if (descriptor_ID == genie::core::record::annotation_parameter_set::DescriptorID::LIKELIHOOD) {
        likelihood_payload.read(reader);
    } else if (descriptor_ID == genie::core::record::annotation_parameter_set::DescriptorID::CONTACT) {
        for (auto i = 0; i < num_chrs; ++i) {
            ContactMatrixBinPayload cmBinPayload;
            cmBinPayload.read(reader);
            cm_bin_payload.push_back(cmBinPayload);
        }
        cm_mat_payload.read(reader);
    } else {
        generic_payload.push_back(static_cast<uint8_t>(reader.read_b(8)));
    }
    reader.flush();
}

void BlockPayload::read(util::BitReader& reader,
                              genie::core::record::annotation_parameter_set::DescriptorID descriptorID,
                              uint8_t numChrs) {
    descriptor_ID = descriptorID;
    num_chrs = numChrs;
    read(reader);
}

void BlockPayload::write(core::Writer& writer) {
    if (descriptor_ID == genie::core::record::annotation_parameter_set::DescriptorID::GENOTYPE) {
        genotype_payload.write(writer);
    } else if (descriptor_ID == genie::core::record::annotation_parameter_set::DescriptorID::LIKELIHOOD) {
        likelihood_payload.write(writer);
    } else if (descriptor_ID == genie::core::record::annotation_parameter_set::DescriptorID::CONTACT) {
        for (auto binPayload : cm_bin_payload) binPayload.write(writer);
        cm_mat_payload.write(writer);
    } else {
         for (const auto& byte : generic_payload) writer.write(byte, 8, true);
    }
    writer.flush();
}

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
