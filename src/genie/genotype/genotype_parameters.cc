/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genotype_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {

// ---------------------------------------------------------------------------------------------------------------------

GenotypeParameters::GenotypeParameters()
    : max_ploidy(0),
      no_reference_flag(false),
      not_available_flag(false),
      binarization_ID(BinarizationID::UNDEFINED),
      concat_axis(ConcatAxis::UNDEFINED),
      variants_payload_params(),
      encode_phases_data_flag(false),
      phases_payload_params(),
      phasing_value(false) {}

// ---------------------------------------------------------------------------------------------------------------------

GenotypeParameters::GenotypeParameters(uint8_t _max_ploidy, bool _no_reference_flag, bool _not_available_flag,
                                       BinarizationID _binarization_ID, ConcatAxis _concat_axis,
                                       std::vector<GenotypeBinMatParameters>&& _variants_payload_params,
                                       bool _encode_phases_data_flag, GenotypeBinMatParameters _phases_payload_params,
                                       bool _phases_value)
    : max_ploidy(_max_ploidy),
      no_reference_flag(_no_reference_flag),
      not_available_flag(_not_available_flag),
      binarization_ID(_binarization_ID),
      concat_axis(_concat_axis),
      variants_payload_params(_variants_payload_params),
      encode_phases_data_flag(_encode_phases_data_flag),
      phases_payload_params(_phases_payload_params),
      phasing_value(_phases_value) {}

// ---------------------------------------------------------------------------------------------------------------------

//GenotypeParameters::GenotypeParameters(GenotypeParameters&& other)
//    : max_ploidy(other.max_ploidy),
//      no_reference_flag(other.no_reference_flag),
//      not_available_flag(other.not_available_flag),
//      binarization_ID(other.binarization_ID),
//      concat_axis(other.concat_axis),
//      variants_payload_params(std::move(other.variants_payload_params)),
//      encode_phases_data_flag(other.encode_phases_data_flag),
//      phases_payload_params(other.phases_payload_params),
//      phasing_value(other.phasing_value) {}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t GenotypeParameters::getMaxPloidy() const { return max_ploidy; }

// ---------------------------------------------------------------------------------------------------------------------

bool GenotypeParameters::getNoRerefernceFlag() const { return no_reference_flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool GenotypeParameters::getNotAvailableFlag() const { return not_available_flag; }

// ---------------------------------------------------------------------------------------------------------------------

BinarizationID GenotypeParameters::getBinarizationID() const { return binarization_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t GenotypeParameters::getNumBitPlanes() const { return static_cast<uint8_t>(variants_payload_params.size()); }

// ---------------------------------------------------------------------------------------------------------------------

ConcatAxis GenotypeParameters::isConcatenated() const { return concat_axis; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<GenotypeBinMatParameters>& GenotypeParameters::getVariantsPayloadParams() const {
    return variants_payload_params;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GenotypeParameters::isPhaseEncoded() const { return encode_phases_data_flag; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t GenotypeParameters::getNumVariantsPayloads() const {
    return static_cast<uint8_t>(variants_payload_params.size());
}

// ---------------------------------------------------------------------------------------------------------------------

const GenotypeBinMatParameters& GenotypeParameters::getPhasesPayloadParams() const { return phases_payload_params; }

// ---------------------------------------------------------------------------------------------------------------------

bool GenotypeParameters::getPhaseValue() const { return phasing_value; }

// ---------------------------------------------------------------------------------------------------------------------

void GenotypeParameters::write(core::Writer& writer) const {
    writer.write(max_ploidy, 8);
    writer.write(no_reference_flag, 1);
    writer.write(not_available_flag, 1);
    writer.write(static_cast<uint8_t>(binarization_ID), 3);
    if (binarization_ID == BinarizationID::BIT_PLANE) {
        writer.write(static_cast<uint8_t>(variants_payload_params.size()), 8);
        writer.write(static_cast<uint8_t>(concat_axis), 8);
    }

    for (auto i = 0; i < static_cast<uint8_t>(variants_payload_params.size()); ++i) {
        writer.write(variants_payload_params[i].sort_rows_flag, 1);
        writer.write(variants_payload_params[i].sort_cols_flag, 1);
        writer.write(variants_payload_params[i].transpose_mat_flag, 1);
        writer.write(static_cast<uint8_t>(variants_payload_params[i].variants_codec_ID), 8);
    }
    writer.write(encode_phases_data_flag, 1);
    if (encode_phases_data_flag) {
        writer.write(phases_payload_params.sort_rows_flag, 1);                  // sort_phases_rows_flag
        writer.write(phases_payload_params.sort_cols_flag, 1);                  // sort_phases_cols_flag
        writer.write(phases_payload_params.transpose_mat_flag, 1);              // transpose_phases_mat_flag
        writer.write(static_cast<uint8_t>(phases_payload_params.variants_codec_ID), 1);  // phases_codec_ID
    } else {
        writer.write(phasing_value, 1);
    }
}
// ---------------------------------------------------------------------------------------------------------------------

size_t GenotypeParameters::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.getBitsWritten();
}
// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------