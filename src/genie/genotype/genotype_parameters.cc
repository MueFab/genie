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

GenotypeParameters::GenotypeParameters():
    max_ploidy(0),
    no_reference_flag(false),
    not_available_flag(false),
    binarization_ID(BinarizationID::UNDEFINED),
    num_bit_planes(0),
    concat_axis(ConcatAxis::UNDEFINED),
    variants_payload_params(),
    encode_phases_data_flag(false),
    phases_payload_params(),
    phases_value(false)
{}
// ---------------------------------------------------------------------------------------------------------------------

GenotypeParameters::GenotypeParameters(
    uint8_t _max_ploidy,
    bool _no_reference_flag,
    bool _not_available_flag,
    BinarizationID _binarization_ID,
    uint8_t _num_bit_planes,
    ConcatAxis _concat_axis,
    std::vector<GenotypePayloadParameters>&& _variants_payload_params,
    bool _encode_phases_data_flag,
    GenotypePayloadParameters&& _phases_payload_params,
    bool _phases_value):
      max_ploidy(_max_ploidy),
      no_reference_flag(_no_reference_flag),
      not_available_flag(_not_available_flag),
      binarization_ID(_binarization_ID),
      num_bit_planes(_num_bit_planes),
      concat_axis(_concat_axis),
      variants_payload_params(_variants_payload_params),
      encode_phases_data_flag(_encode_phases_data_flag),
      phases_payload_params(_phases_payload_params),
      phases_value(_phases_value)
{}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t GenotypeParameters::getMaxPloidy() const { return max_ploidy; }

// ---------------------------------------------------------------------------------------------------------------------

bool GenotypeParameters::getNoRerefernceFlag() const { return no_reference_flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool GenotypeParameters::getNotAvailableFlag() const { return not_available_flag; }

// ---------------------------------------------------------------------------------------------------------------------

BinarizationID GenotypeParameters::getBinarizationID() const { return binarization_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t GenotypeParameters::getNumBitPlanes() const {return num_bit_planes;}

// ---------------------------------------------------------------------------------------------------------------------

ConcatAxis GenotypeParameters::isConcatenated() const { return concat_axis; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<GenotypePayloadParameters>& GenotypeParameters::getVariantsPayloadParams() const {return variants_payload_params;}

// ---------------------------------------------------------------------------------------------------------------------

bool GenotypeParameters::isPhaseEncoded() const { return encode_phases_data_flag; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t GenotypeParameters::getNumVariantsPayloads() const {return static_cast<uint8_t>(variants_payload_params.size());}

// ---------------------------------------------------------------------------------------------------------------------

const GenotypePayloadParameters& GenotypeParameters::getPhasesPayloadParams() const { return phases_payload_params; }

// ---------------------------------------------------------------------------------------------------------------------

bool GenotypeParameters::getPhaseValue() const { return phases_value; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------