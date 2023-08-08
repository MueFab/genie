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