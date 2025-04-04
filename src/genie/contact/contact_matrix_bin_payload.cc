/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

//#include "contact_matrix_bin_payload.h"

/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
 */

#include "genie/contact/contact_matrix_bin_payload.h"
#include <fstream>
#include <vector>
#include <genie/util/bit_reader.h>
#include <genie/util/bit_writer.h>
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::contact {

ContactMatrixBinPayload::ContactMatrixBinPayload()
    : sample_ID_(0),
      chr_ID_(0),
      bin_size_multiplier_(0),
      weight_value_(){}

// -----------------------------------------------------------------------------

ContactMatrixBinPayload::ContactMatrixBinPayload(
    uint16_t sample_ID,
    uint8_t chr_ID,
    uint32_t bin_size_multiplier,
    std::vector<std::vector<double_t>>&& weight_value
) : sample_ID_(sample_ID),
    chr_ID_(chr_ID),
    bin_size_multiplier_(bin_size_multiplier),
    weight_value_(std::move(weight_value)) {}

// -----------------------------------------------------------------------------

ContactMatrixBinPayload::ContactMatrixBinPayload(const ContactMatrixBinPayload& other) = default;

// -----------------------------------------------------------------------------

ContactMatrixBinPayload::ContactMatrixBinPayload(ContactMatrixBinPayload&& other) noexcept
    : sample_ID_(other.sample_ID_), chr_ID_(other.chr_ID_), bin_size_multiplier_(other.bin_size_multiplier_),
      weight_value_(std::move(other.weight_value_)) {}

// -----------------------------------------------------------------------------

ContactMatrixBinPayload& ContactMatrixBinPayload::operator=(const ContactMatrixBinPayload& other) = default;

// -----------------------------------------------------------------------------

ContactMatrixBinPayload& ContactMatrixBinPayload::operator=(ContactMatrixBinPayload&& other) noexcept {
  if (this != &other) {
    sample_ID_ = other.sample_ID_;
    chr_ID_ = other.chr_ID_;
    bin_size_multiplier_ = other.bin_size_multiplier_;
    weight_value_ = std::move(other.weight_value_);
  }
  return *this;
}

// -----------------------------------------------------------------------------

ContactMatrixBinPayload::ContactMatrixBinPayload(
    genie::util::BitReader& bit_reader,
    uint8_t num_norm_methods,
    uint32_t num_bin_entries
) {

  sample_ID_ = bit_reader.Read<uint16_t>();
  chr_ID_ = bit_reader.Read<uint8_t>();
  bin_size_multiplier_ = bit_reader.Read<uint32_t>();

  weight_value_.resize(num_norm_methods);
  for (size_t i = 0; i < num_norm_methods; ++i) {
    weight_value_[i].resize(num_bin_entries);

    for (size_t j = 0; j < num_bin_entries; ++j) {
      auto weight_value = bit_reader.ReadAlignedInt<uint64_t>();
      weight_value_[i][j] = *reinterpret_cast<double*>(&weight_value);
    }
  }
}

// -----------------------------------------------------------------------------

bool ContactMatrixBinPayload::operator==(const ContactMatrixBinPayload& other) const {
  // Check if the sizes match first
  if (sample_ID_ != other.sample_ID_ ||
      chr_ID_ != other.chr_ID_ ||
      bin_size_multiplier_ != other.bin_size_multiplier_ ||
      weight_value_.size() != other.weight_value_.size()) {
    return false;
  }

  // Compare each element, treating NaN as equal
  for (size_t i = 0; i < weight_value_.size(); ++i) {
    if (weight_value_[i].size() != other.weight_value_[i].size()) {
      return false;
    }
    for (size_t j = 0; j < weight_value_[i].size(); ++j) {
      // Check if both are NaN or both are equal
      if (std::isnan(weight_value_[i][j]) && std::isnan(other.weight_value_[i][j])) {
        continue;
      }
      if (!std::isnan(weight_value_[i][j]) && !std::isnan(other.weight_value_[i][j]) &&
          weight_value_[i][j] != other.weight_value_[i][j]) {
        return false;
      }
    }
  }

  return true;
}

// -----------------------------------------------------------------------------

uint16_t ContactMatrixBinPayload::GetSampleID() const { return sample_ID_; }

// -----------------------------------------------------------------------------

uint8_t ContactMatrixBinPayload::GetChrID() const { return chr_ID_; }

// -----------------------------------------------------------------------------

uint32_t ContactMatrixBinPayload::GetBinSizeMultiplier() const { return bin_size_multiplier_; }

// -----------------------------------------------------------------------------

size_t ContactMatrixBinPayload::GetNumNormMethods() const {
  return weight_value_.size();
}

// -----------------------------------------------------------------------------

const std::vector<std::vector<double_t>>& ContactMatrixBinPayload::GetWeightValue() const { return weight_value_; }

// -----------------------------------------------------------------------------

void ContactMatrixBinPayload::SetSampleID(uint16_t sample_id) {
  sample_ID_ = sample_id;
}

// -----------------------------------------------------------------------------

void ContactMatrixBinPayload::SetChrID(uint8_t chr_id) {
  chr_ID_ = chr_id;
}

// -----------------------------------------------------------------------------

void ContactMatrixBinPayload::SetBinSizeMultiplier(uint32_t bin_size_multiplier) {
  bin_size_multiplier_ = bin_size_multiplier;
}

// -----------------------------------------------------------------------------

void ContactMatrixBinPayload::SetWeightValue(std::vector<std::vector<double_t>>&& weight_value) {
  weight_value_ = std::move(weight_value);
}

void ContactMatrixBinPayload::ReadWeightValuesFromFile(const std::string& fpath) {
  ReadWeightValuesFromFileAtIndex(fpath, weight_value_.size());
}

// -----------------------------------------------------------------------------

void ContactMatrixBinPayload::ReadWeightValuesFromFileAtIndex(const std::string& fpath, size_t idx) {
  std::ifstream reader(fpath, std::ios::binary);
  if (reader.fail()) {
    throw std::runtime_error("Failed to open file: " + fpath);
  }

  genie::util::BitReader bitreader(reader);

  // Get file size
  reader.seekg(0, std::ios::end);
  size_t file_size = static_cast<size_t>(reader.tellg());
  reader.seekg(0, std::ios::beg);

  // Calculate number of entries
  size_t num_entries = file_size / sizeof(double);

  // Resize the weight_value_ vector to ensure it can accommodate idx
  if (weight_value_.size() <= idx) {
    weight_value_.resize(idx + 1);
  }

  while (bitreader.IsStreamGood() && num_entries > 0) {
    auto weight_value = bitreader.ReadAlignedInt<uint64_t>();
    weight_value_.at(idx).emplace_back(*reinterpret_cast<double*>(&weight_value));
    num_entries--;
  }
}

// -----------------------------------------------------------------------------

void ContactMatrixBinPayload::Write(util::BitWriter& writer) const {
  UTILS_DIE_IF(!writer.IsByteAligned(), "Not byte aligned!");
  writer.WriteBypassBE(GetSampleID());
  writer.WriteBypassBE(GetChrID());
  writer.WriteBypassBE(GetBinSizeMultiplier());

  for (const auto &norm_weights: GetWeightValue()){
    for (const auto &weight_value: norm_weights){
      const uint64_t bits = *reinterpret_cast<const uint64_t*>(&weight_value);
      writer.WriteBypassBE(bits);
    }
  }
}


// -----------------------------------------------------------------------------

}  // namespace genie::contact

// -----------------------------------------------------------------------------

