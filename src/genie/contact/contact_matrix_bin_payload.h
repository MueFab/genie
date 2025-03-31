/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#ifndef GENIE_CONTACT_CONTACT_MATRIX_BIN_PAYLOAD_H
#define GENIE_CONTACT_CONTACT_MATRIX_BIN_PAYLOAD_H

#include <genie/core/constants.h>
#include <genie/core/writer.h>
#include <genie/util/bit_reader.h>
#include <genie/util/bit_writer.h>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::contact {

// ---------------------------------------------------------------------------------------------------------------------

class [[maybe_unused]] ContactMatrixBinPayload {
 private:
    uint16_t sample_ID_;
    uint8_t chr_ID_;
    uint32_t bin_size_multiplier_;
    std::vector<std::vector<double_t>> weight_value_;

 public:
  // Constructors
  ContactMatrixBinPayload() = default;
  ContactMatrixBinPayload(
      uint16_t  sample_ID,
      uint8_t  chr_ID,
      uint32_t  bin_size_multiplier,
      std::vector<std::vector<double_t>>&& weight_value
  );
  ContactMatrixBinPayload(const ContactMatrixBinPayload& other);
  ContactMatrixBinPayload(ContactMatrixBinPayload&& other) noexcept;
  ~ContactMatrixBinPayload() = default;

  // Assignment operators
  ContactMatrixBinPayload& operator=(const ContactMatrixBinPayload& other);
  ContactMatrixBinPayload& operator=(ContactMatrixBinPayload&& other) noexcept;

  // BitReader constructor
  explicit ContactMatrixBinPayload(
      genie::util::BitReader& bit_reader,
      uint8_t num_norm_methods,
      uint32_t num_bin_entries
  );

  bool operator==(const ContactMatrixBinPayload& other) const;

  // Getters
  [[nodiscard]] uint16_t GetSampleID() const;
  [[nodiscard]] uint8_t GetChrID() const;
  [[nodiscard]] uint32_t GetBinSizeMultiplier() const;
  [[nodiscard]] const std::vector<std::vector<double_t>>& GetWeightValue() const;

  // Setters
  void SetSampleID(uint16_t sample_id);
  void SetChrID(uint8_t chr_id);
  void SetBinSizeMultiplier(uint32_t bin_size_multiplier);
  void SetWeightValue(std::vector<std::vector<double_t>>&& weight_value);

  void ParseWeightValues(const std::string& fpath);
  void ParseWeightValues(const std::string& fpath, size_t idx);

  void Write(util::BitWriter& writer) const;

};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::contact

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_CONTACT_MATRIX_BIN_PAYLOAD_H

// ---------------------------------------------------------------------------------------------------------------------
