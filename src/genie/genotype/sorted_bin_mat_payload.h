/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SRC_GENIE_GENOTYPE_SORTED_BIN_MAT_PAYLOAD_H_
#define GENIE_SRC_GENIE_GENOTYPE_SORTED_BIN_MAT_PAYLOAD_H_

// -----------------------------------------------------------------------------

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

#include "genie/genotype/bin_mat_payload.h"
#include "genie/genotype/row_col_ids_payload.h"
//#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::genotype {

// -----------------------------------------------------------------------------

class [[maybe_unused]] SortedBinMatPayload {
 private:
  BinMatPayload bin_mat_payload_;
  std::optional<RowColIdsPayload> row_ids_payload_;
  std::optional<RowColIdsPayload> col_ids_payload_;

 public:
  // Default Constructors
  SortedBinMatPayload();

  // Parameterized constructor
  [[maybe_unused]] SortedBinMatPayload(
      BinMatPayload&& bin_mat_payload,
      std::optional<RowColIdsPayload>&& row_ids_payload,
      std::optional<RowColIdsPayload>&& col_ids_payload
  );

  // Copy constructor
  SortedBinMatPayload(const SortedBinMatPayload& other);

  // Move constructor
  SortedBinMatPayload(SortedBinMatPayload&& other) noexcept;

  // Copy assignment operator
  SortedBinMatPayload& operator=(const SortedBinMatPayload& other);

  // Move assignment operator
  SortedBinMatPayload& operator=(SortedBinMatPayload&& other) noexcept;

  // Constructor from BitReader
  [[maybe_unused]] explicit SortedBinMatPayload(
    util::BitReader& reader,
    core::AlgoID codec_ID,
    bool rows_sorted,
    bool cols_sorted
  );

  // Getters
  [[maybe_unused]] const BinMatPayload& GetBinMatPayload() const;
  bool IsRowsSorted() const;
  bool IsColsSorted() const;
  const std::optional<RowColIdsPayload>& GetRowIdsPayload() const;
  const std::optional<RowColIdsPayload>& GetColIdsPayload() const;

  // Setters
  [[maybe_unused]] void SetBinMatPayload(BinMatPayload& bin_mat_payload);
  [[maybe_unused]] void SetRowIdsPayload(std::optional<RowColIdsPayload>&& row_ids_payload);
  [[maybe_unused]] void SetColIdsPayload(std::optional<RowColIdsPayload>&& col_ids_payload);

  // Size calculation
  size_t GetSize() const;

  // Function to write to writer
  void Write(util::BitWriter& writer) const;
  void Write(core::Writer& writer) const;

};

// -----------------------------------------------------------------------------

}  // namespace genie::genotype

// -----------------------------------------------------------------------------

#endif  // GENIE_SRC_GENIE_GENOTYPE_SORTED_BIN_MAT_PAYLOAD_H_

// -----------------------------------------------------------------------------
