/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#ifndef GENIE_CONTACT_CONTACT_MATRIX_BIN_PAYLOAD_H
#define GENIE_CONTACT_CONTACT_MATRIX_BIN_PAYLOAD_H

#include <genie/core/constants.h>
#include <genie/core/writer.h>
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

//TODO(yeremia): Docstring
class ContactMatrixBinPayload {
 private:
    uint16_t sample_ID;
    uint8_t  chr_ID;
    uint32_t bin_size_multiplier;
    std::vector<std::vector<double_t>> weight_value;

 public:
    ContactMatrixBinPayload();

    ContactMatrixBinPayload(const ContactMatrixBinPayload& other) noexcept;

    ContactMatrixBinPayload(ContactMatrixBinPayload&& other) noexcept;

    ContactMatrixBinPayload& operator=(ContactMatrixBinPayload&& other) noexcept;

    explicit ContactMatrixBinPayload(util::BitReader &reader);

    uint16_t getSampleID() const;
    uint8_t getChrID() const;
    uint32_t getBinSizeMultiplier() const;
    const std::vector<std::vector<double_t>>& getWeightValue() const;

    void setSampleID(uint16_t sample_ID);
    void setChrID(uint8_t chr_ID);
    void setBinSizeMultipler(uint32_t bin_size_multiplier);
    void setWeightValue(std::vector<std::vector<double_t>>&& weight_value);

    void write(util::BitWriter& writer) const;

};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_CONTACT_MATRIX_BIN_PAYLOAD_H

// ---------------------------------------------------------------------------------------------------------------------
