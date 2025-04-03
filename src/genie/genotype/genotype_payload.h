/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GENOTYPE_PAYLOAD_H
#define GENIE_GENOTYPE_PAYLOAD_H

#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/core/writer.h"
#include "genie/genotype/amax_payload.h"
#include "genie/genotype/row_col_ids_payload.h"
#include "genie/genotype/amax_payload.h"
#include "genie/genotype/sorted_bin_mat_payload.h"
#include "genie/genotype/genotype_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::genotype {

// ---------------------------------------------------------------------------------------------------------------------

class GenotypePayload {
 private:
  uint8_t max_ploidy_;
  // reserved u(5)
  bool no_reference_flag_;
  bool not_available_flag_;
  bool phases_value_;
//  uint8_t num_bit_planes_; //? Only for bit plane binarization

  std::vector<SortedBinMatPayload> variants_payloads_;
  std::optional<AmaxPayload> variants_amax_payload_;
  std::optional<SortedBinMatPayload> phases_payload_;

 public:
  // Default constructor
  GenotypePayload();

  // Parameterized constructor
  explicit GenotypePayload(
      uint8_t max_ploidy,
      bool no_reference_flag,
      bool not_available_flag,
      bool phases_value,
      std::vector<SortedBinMatPayload>&& variants_payload,
      std::optional<AmaxPayload>&& variants_amax_payload = std::nullopt,
      std::optional<SortedBinMatPayload>&& phases_payload = std::nullopt);

  // Copy constructor
  GenotypePayload(const GenotypePayload& other);

  // Move constructor
  GenotypePayload(GenotypePayload&& other) noexcept;

  // Copy assignment operator
  GenotypePayload& operator=(const GenotypePayload& other);

  // Move assignment operator
  GenotypePayload& operator=(GenotypePayload&& other) noexcept;

  // Constructor from BitReader
  explicit GenotypePayload(util::BitReader& reader,
                           [[maybe_unused]] GenotypeParameters& parameters);

  // Equality operator
  bool operator==(const GenotypePayload& other) const;

  // Getters
  [[maybe_unused]] uint8_t GetMaxPloidy() const;
  bool GetNoReferenceFlag() const;
  bool GetNotAvailableFlag() const;
  bool GetPhasesValue() const;
  uint8_t GetNumBitPlanes() const;
  const std::vector<SortedBinMatPayload>& GetVariantsPayloads() const;
  bool IsAmaxPayloadExist() const;
  const std::optional<AmaxPayload>& GetVariantsAmaxPayload() const;
  bool EncodePhaseValues() const;
  const std::optional<SortedBinMatPayload>& GetPhasesPayload() const;

  // Setters
  void SetMaxPloidy(uint8_t max_ploidy);
  void SetNoReferenceFlag(bool no_reference_flag);
  void SetNotAvailableFlag(bool not_available_flag);
  void SetPhasesValue(bool phases_value);
  void AddVariantsPayload(SortedBinMatPayload&& payload);
  void AddVariantsPayload(const SortedBinMatPayload& payload);
  void SetVariantsPayloads(std::vector<SortedBinMatPayload>&& variants_payload);
  void SetVariantsAmaxPayload(std::optional<AmaxPayload>&& variants_amax_payload);
  void SetPhasesPayload(std::optional<SortedBinMatPayload>&& phases_payload);

  // GetSize function
  [[nodiscard]] size_t GetSize() const;

  // Function to write to writer
  void Write(util::BitWriter& writer) const;
  void Write(core::Writer& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::genotype

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GENOTYPE_PAYLOAD_H

// ---------------------------------------------------------------------------------------------------------------------
