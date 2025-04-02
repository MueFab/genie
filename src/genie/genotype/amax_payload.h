/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SRC_GENIE_GENOTYPE_AMAX_PAYLOAD_H_
#define GENIE_SRC_GENIE_GENOTYPE_AMAX_PAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <optional>

#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/core/writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::genotype {

// ---------------------------------------------------------------------------------------------------------------------

class AmaxPayload {
 private:
 std::vector<uint64_t> amax_elements_;
 
 uint8_t nbits_per_elem_;

 public:
  // Default constructor
  AmaxPayload();

  // Parameterized constructor
  explicit AmaxPayload(
    std::vector<uint64_t>&& amax_elements,
    std::optional<uint8_t> nbits_per_elem = std::nullopt
  );

  // Copy constructor
  AmaxPayload(const AmaxPayload& other);

  // Move constructor
  AmaxPayload(AmaxPayload&& other) noexcept;

  // Copy assignment operator
  AmaxPayload& operator=(const AmaxPayload& other);

  // Move assignment operator
  AmaxPayload& operator=(AmaxPayload&& other) noexcept;

  // Constructor from BitReader
  explicit AmaxPayload(genie::util::BitReader& reader);

  // Getter methods
  size_t GetNElems() const;
  uint8_t GetNBitsPerElem() const;
  const std::vector<uint64_t>& GetAmaxElements() const;

  // Setter methods
  [[maybe_unused]] void SetNbitsPerElem(uint8_t nbits_per_elem);
  [[maybe_unused]] void SetAmaxElements(std::vector<uint64_t>&& amax_elements);

  // GetSize method
  size_t GetSize() const;

  // Function to write to writer
  void Write(util::BitWriter& writer) const;
  void Write(core::Writer& writer) const;

  static uint8_t ComputeNbitsPerElement(const std::vector<uint64_t>& amax_elements);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::genotype

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SRC_GENIE_GENOTYPE_AMAX_PAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------
