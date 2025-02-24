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
    : max_ploidy_(0),
      no_reference_flag_(false),
      not_available_flag_(false),
      binarization_ID_(BinarizationID::UNDEFINED),
      num_bit_plane_(0),
      concat_axis_(ConcatAxis::UNDEFINED),
      variants_payload_params_(),
      encode_phases_data_flag_(false),
      phases_payload_params_(),
      phasing_value_(false) {}

// ---------------------------------------------------------------------------------------------------------------------

// Parameterized constructor
GenotypeParameters::GenotypeParameters(uint8_t max_ploidy, bool no_reference_flag, bool not_available_flag,
  BinarizationID binarization_ID, uint8_t num_bit_planes, ConcatAxis concat_axis,
  std::vector<GenotypeBinMatParameters>&& variants_payload_params, bool encode_phases_data_flag,
  GenotypeBinMatParameters phases_payload_params, bool phases_value)
: max_ploidy_(max_ploidy),
      no_reference_flag_(no_reference_flag),
      not_available_flag_(not_available_flag),
      binarization_ID_(binarization_ID),
      num_bit_plane_(num_bit_planes),
      concat_axis_(concat_axis),
      variants_payload_params_(std::move(variants_payload_params)),
      encode_phases_data_flag_(encode_phases_data_flag),
      phases_payload_params_(std::move(phases_payload_params)),
      phasing_value_(phases_value) {}

// ---------------------------------------------------------------------------------------------------------------------

// Move constructor
GenotypeParameters::GenotypeParameters(GenotypeParameters&& other) noexcept
    : max_ploidy_(other.max_ploidy_),
      no_reference_flag_(other.no_reference_flag_),
      not_available_flag_(other.not_available_flag_),
      binarization_ID_(other.binarization_ID_),
      num_bit_plane_(other.num_bit_plane_),
      concat_axis_(other.concat_axis_),
      variants_payload_params_(std::move(other.variants_payload_params_)),
      encode_phases_data_flag_(other.encode_phases_data_flag_),
      phases_payload_params_(std::move(other.phases_payload_params_)),
      phasing_value_(other.phasing_value_) {}

// ---------------------------------------------------------------------------------------------------------------------

// Copy constructor
GenotypeParameters::GenotypeParameters(const GenotypeParameters& other)
    : max_ploidy_(other.max_ploidy_),
      no_reference_flag_(other.no_reference_flag_),
      not_available_flag_(other.not_available_flag_),
      binarization_ID_(other.binarization_ID_),
      num_bit_plane_(other.num_bit_plane_),
      concat_axis_(other.concat_axis_),
      variants_payload_params_(other.variants_payload_params_),
      encode_phases_data_flag_(other.encode_phases_data_flag_),
      phases_payload_params_(other.phases_payload_params_),
      phasing_value_(other.phasing_value_) {}

// ---------------------------------------------------------------------------------------------------------------------

// Move assignment operator
GenotypeParameters& GenotypeParameters::operator=(GenotypeParameters&& other) noexcept {
  if (this != &other) {
    max_ploidy_ = other.max_ploidy_;
    no_reference_flag_ = other.no_reference_flag_;
    not_available_flag_ = other.not_available_flag_;
    binarization_ID_ = other.binarization_ID_;
    num_bit_plane_ = other.num_bit_plane_;
    concat_axis_ = other.concat_axis_;
    variants_payload_params_ = std::move(other.variants_payload_params_);
    encode_phases_data_flag_ = other.encode_phases_data_flag_;
    phases_payload_params_ = std::move(other.phases_payload_params_);
    phasing_value_ = other.phasing_value_;
  }
  return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

// Copy assignment operator
GenotypeParameters& GenotypeParameters::operator=(const GenotypeParameters& other) {
  if (this != &other) {
    max_ploidy_ = other.max_ploidy_;
    no_reference_flag_ = other.no_reference_flag_;
    not_available_flag_ = other.not_available_flag_;
    binarization_ID_ = other.binarization_ID_;
    num_bit_plane_ = other.num_bit_plane_;
    concat_axis_ = other.concat_axis_;
    variants_payload_params_ = other.variants_payload_params_;
    encode_phases_data_flag_ = other.encode_phases_data_flag_;
    phases_payload_params_ = other.phases_payload_params_;
    phasing_value_ = other.phasing_value_;
  }
  return *this;
}


// ---------------------------------------------------------------------------------------------------------------------

uint8_t GenotypeParameters::GetMaxPloidy() const { return max_ploidy_; }

// ---------------------------------------------------------------------------------------------------------------------

bool GenotypeParameters::GetNoReferenceFlag() const { return no_reference_flag_; }

// ---------------------------------------------------------------------------------------------------------------------

bool GenotypeParameters::GetNotAvailableFlag() const { return not_available_flag_; }

// ---------------------------------------------------------------------------------------------------------------------

BinarizationID GenotypeParameters::GetBinarizationID() const { return binarization_ID_; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t GenotypeParameters::GetNumBitPlanes() const { return num_bit_plane_; }

// ---------------------------------------------------------------------------------------------------------------------

ConcatAxis GenotypeParameters::GetConcatAxis() const { return concat_axis_; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t GenotypeParameters::GetNumVariantsPayloads() const {return static_cast<uint8_t>(variants_payload_params_.size());}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<GenotypeBinMatParameters>& GenotypeParameters::GetVariantsPayloadParams() const {return variants_payload_params_;}

// ---------------------------------------------------------------------------------------------------------------------

bool GenotypeParameters::IsPhaseEncoded() const { return encode_phases_data_flag_; }

// ---------------------------------------------------------------------------------------------------------------------

const GenotypeBinMatParameters& GenotypeParameters::GetPhasesPayloadParams() const { return phases_payload_params_; }

// ---------------------------------------------------------------------------------------------------------------------

bool GenotypeParameters::GetPhaseValue() const { return phasing_value_; }

// ---------------------------------------------------------------------------------------------------------------------

void GenotypeParameters::Write(core::Writer& writer) const {
  writer.Write(max_ploidy_, 8);
    writer.Write(no_reference_flag_, 1);
    writer.Write(not_available_flag_, 1);
    writer.Write(static_cast<uint8_t>(binarization_ID_), 3);
    if (binarization_ID_ == BinarizationID::BIT_PLANE) {
      writer.Write(GetNumBitPlanes(), 8);// was GetNumVariantsPayloads
        writer.Write(static_cast<uint8_t>(concat_axis_), 8);
    }

    for (auto i = 0u; i < GetNumVariantsPayloads(); ++i) {
      writer.Write(variants_payload_params_[i].sort_rows_flag, 1);
        writer.Write(variants_payload_params_[i].sort_cols_flag, 1);
        writer.Write(variants_payload_params_[i].transpose_mat_flag, 1);
        writer.Write(
            static_cast<uint8_t>(variants_payload_params_[i].variants_codec_ID),
            8);
    }
    writer.Write(encode_phases_data_flag_, 1);
    if (encode_phases_data_flag_) {
      writer.Write(phases_payload_params_.sort_rows_flag,
                   1);                  // sort_phases_rows_flag
        writer.Write(phases_payload_params_.sort_cols_flag,
                     1);                  // sort_phases_cols_flag
        writer.Write(phases_payload_params_.transpose_mat_flag,
                     1);              // transpose_phases_mat_flag
        writer.Write(
            static_cast<uint8_t>(phases_payload_params_.variants_codec_ID),
            8);  // phases_codec_ID
    } else {
      writer.Write(phasing_value_, 1);
    }
    writer.Flush();
}

// ---------------------------------------------------------------------------------------------------------------------

size_t GenotypeParameters::GetSize(core::Writer& writesize) const {
  Write(writesize);
    return writesize.GetBitsWritten();
}

// ---------------------------------------------------------------------------------------------------------------------

// Read from BitReader
void GenotypeParameters::read(util::BitReader& reader) {
  // Placeholder implementation
  // Actual reading logic should be implemented here
  (void)reader;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------