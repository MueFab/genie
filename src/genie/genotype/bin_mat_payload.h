/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#ifndef GENIE_SRC_GENIE_GENOTYPE_BIN_MAT_PAYLOAD_H_
#define GENIE_SRC_GENIE_GENOTYPE_BIN_MAT_PAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::genotype {

// ---------------------------------------------------------------------------------------------------------------------

class BinMatPayload {
 private:
  core::AlgoID codec_ID_;
  std::vector<uint8_t> payload_;  // JBIG or CABAC: ISO/IEC 11544 or ISO/IEC 23092-2
  uint32_t nrows_;
  uint32_t ncols_;
//  std::vector<uint8_t> compressed_payload_;

 public:
  // Default constructor
  BinMatPayload();

  // Original constructor with all parameters
  BinMatPayload(
      core::AlgoID codec_id,
      std::vector<uint8_t>&& payload,
      uint32_t nrows,
      uint32_t ncols
  );

  // Copy constructor
  BinMatPayload(const BinMatPayload& other);

  // Move constructor
  BinMatPayload(BinMatPayload&& other) noexcept;

  // Constructor from bitreader
  BinMatPayload(
      util::BitReader& reader,
      size_t payload_size,
      core::AlgoID codec_ID
  );

  // Copy assignment operator
  BinMatPayload& operator=(const BinMatPayload& other);

  // Move assignment operator
  BinMatPayload& operator=(BinMatPayload&& other) noexcept;

  // Comparison operator
  bool operator==(const BinMatPayload& other) const;

  // Getters
  [[maybe_unused]] core::AlgoID GetCodecID() const;
  [[maybe_unused]] const std::vector<uint8_t>& GetPayload() const;
  uint32_t GetNRows() const;
  uint32_t GetNCols() const;
//  [[maybe_unused]] const std::vector<uint8_t>& GetCompressedPayload() const;

  // Setters
  [[maybe_unused]] void SetCodecID(core::AlgoID codec_id);
  [[maybe_unused]] void SetPayload(std::vector<uint8_t>&& payload);
//  [[maybe_unused]] void SetNRows(uint32_t nrows);
//  [[maybe_unused]] void SetNCols(uint32_t ncols);
//  [[maybe_unused]] void SetCompressedPayload(std::vector<uint8_t>&& compressed_payload);

  // Methods
  [[maybe_unused]] size_t GetPayloadSize() const;
  size_t GetSize() const;
//  void WriteCompressed(core::Writer& writer) const;

  // Function to write to writer
  void Write(util::BitWriter writer) const;
  void Write(core::Writer& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::genotype

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SRC_GENIE_GENOTYPE_BIN_MAT_PAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------
