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

//TODO(yeremia): Docstring
class [[maybe_unused]] ContactMatrixBinPayload {
 private:
    uint16_t sample_ID_;
    uint8_t chr_ID_;
    uint32_t bin_size_multiplier_;
    std::vector<std::vector<double_t>> weight_value_;

 public:
    ContactMatrixBinPayload();

    ContactMatrixBinPayload(const ContactMatrixBinPayload& other) noexcept;

    ContactMatrixBinPayload(ContactMatrixBinPayload&& other) noexcept;

    ContactMatrixBinPayload& operator=(ContactMatrixBinPayload&& other) noexcept;

    explicit ContactMatrixBinPayload(util::BitReader &reader);

    uint16_t GetSampleID() const;
    uint8_t GetChrId() const;
    uint32_t GetBinSizeMultiplier() const;
    const std::vector<std::vector<double_t>>& GetWeightValue() const;

    void SetSampleID(uint16_t sample_ID);
    void SetChrID(uint8_t chr_ID);
    void SetBinSizeMultipler(uint32_t bin_size_multiplier);
    void SetWeightValue(std::vector<std::vector<double_t>>&& weight_value);

    void Write(util::BitWriter& writer) const;

};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::contact

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_CONTACT_MATRIX_BIN_PAYLOAD_H

// ---------------------------------------------------------------------------------------------------------------------
