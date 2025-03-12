/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
 */

#include "sorted_bin_mat_payload.h"
#include <optional>

// -----------------------------------------------------------------------------

namespace genie::genotype {

// -----------------------------------------------------------------------------

// Default constructor
SortedBinMatPayload::SortedBinMatPayload() = default;

// -----------------------------------------------------------------------------

// Parameterized constructor
[[maybe_unused]] SortedBinMatPayload::SortedBinMatPayload(
    BinMatPayload&& bin_mat_payload,
    std::optional<RowColIdsPayload>&& row_ids_payload,
    std::optional<RowColIdsPayload>&& col_ids_payload
): bin_mat_payload_(bin_mat_payload),
   row_ids_payload_(row_ids_payload),
   col_ids_payload_(col_ids_payload) {}

// -----------------------------------------------------------------------------

// Copy constructor
SortedBinMatPayload::SortedBinMatPayload(const SortedBinMatPayload& other) {
  bin_mat_payload_ = other.bin_mat_payload_;
  row_ids_payload_ = other.row_ids_payload_;
  col_ids_payload_ = other.col_ids_payload_;
}

// -----------------------------------------------------------------------------

// Move constructor
SortedBinMatPayload::SortedBinMatPayload(SortedBinMatPayload&& other) noexcept {
  bin_mat_payload_ = std::move(other.bin_mat_payload_);
  row_ids_payload_ = std::move(other.row_ids_payload_);
  col_ids_payload_ = std::move(other.col_ids_payload_);
}

// -----------------------------------------------------------------------------

// Copy assignment operator
SortedBinMatPayload& SortedBinMatPayload::operator=(
    const SortedBinMatPayload& other) {
  if (this != &other) {
    bin_mat_payload_ = other.bin_mat_payload_;
    row_ids_payload_ = other.row_ids_payload_;
    col_ids_payload_ = other.col_ids_payload_;
  }
  return *this;
}

// -----------------------------------------------------------------------------

// Move assignment operator
SortedBinMatPayload& SortedBinMatPayload::operator=(
    SortedBinMatPayload&& other) noexcept {
  if (this != &other) {
    bin_mat_payload_ = std::move(other.bin_mat_payload_);
    row_ids_payload_ = std::move(other.row_ids_payload_);
    col_ids_payload_ = std::move(other.col_ids_payload_);
  }
  return *this;
}

// -----------------------------------------------------------------------------

// Constructor from BitReader
[[maybe_unused]] SortedBinMatPayload::SortedBinMatPayload(
    util::BitReader& reader,
    core::AlgoID codec_ID,
    bool rows_sorted,
    bool cols_sorted
): bin_mat_payload_(reader, static_cast<size_t>(reader.Read<uint32_t>()), codec_ID) {

  if (rows_sorted) {
    auto row_ids_payload_size = static_cast<size_t>(reader.Read<uint32_t>());
    row_ids_payload_ = RowColIdsPayload(reader, row_ids_payload_size);
  }

  if (cols_sorted) {
    auto col_ids_payload_size = static_cast<size_t>(reader.Read<uint32_t>());
    row_ids_payload_ = RowColIdsPayload(reader, col_ids_payload_size);
  }
}

// -----------------------------------------------------------------------------

[[maybe_unused]] const BinMatPayload& SortedBinMatPayload::GetBinMatPayload() const {
  return bin_mat_payload_;
}

// -----------------------------------------------------------------------------

bool SortedBinMatPayload::IsRowsSorted() const {
  return row_ids_payload_.has_value();
}

// -----------------------------------------------------------------------------

bool SortedBinMatPayload::IsColsSorted() const {
  return col_ids_payload_.has_value();
}

// -----------------------------------------------------------------------------

const std::optional<RowColIdsPayload>& SortedBinMatPayload::GetRowIdsPayload()
    const {
  return row_ids_payload_;
}

// -----------------------------------------------------------------------------

const std::optional<RowColIdsPayload>& SortedBinMatPayload::GetColIdsPayload()
    const {
  return col_ids_payload_;
}

// -----------------------------------------------------------------------------

[[maybe_unused]] void SortedBinMatPayload::SetBinMatPayload(BinMatPayload& bin_mat_payload) {
  bin_mat_payload_ = std::move(bin_mat_payload);
}

// -----------------------------------------------------------------------------

[[maybe_unused]] void SortedBinMatPayload::SetRowIdsPayload(std::optional<RowColIdsPayload>&& row_ids_payload) {
  row_ids_payload_ = std::move(row_ids_payload);
}

// -----------------------------------------------------------------------------

[[maybe_unused]] [[maybe_unused]] void SortedBinMatPayload::SetColIdsPayload(std::optional<RowColIdsPayload>&& col_ids_payload) {
  col_ids_payload_ = std::move(col_ids_payload);
}

// -----------------------------------------------------------------------------

// Size calculation
size_t SortedBinMatPayload::GetSize() const {
  size_t size = 0;
  size += sizeof(uint32_t); //bin_mat_payload_size u(32)
  size += bin_mat_payload_.GetSize();
  if (IsRowsSorted()) {
    size += sizeof(uint32_t); //row_ids_payload_size u(32)
    size += GetRowIdsPayload()->GetSize();
  }
  if (IsColsSorted()) {
    size += sizeof(uint32_t); //col_ids_payload_size u(32)
    size += GetColIdsPayload()->GetSize();
  }
  return size;
}

// -----------------------------------------------------------------------------

void SortedBinMatPayload::Write(util::BitWriter& writer) const {
  UTILS_DIE_IF(!writer.IsByteAligned(), "Byte is not aligned!");

  writer.WriteBypassBE(bin_mat_payload_.GetSize());
  bin_mat_payload_.Write(writer);

  if (IsRowsSorted()){
    writer.WriteBypassBE(static_cast<uint32_t>(GetRowIdsPayload()->GetSize()));
    GetRowIdsPayload()->Write(writer);
  }

  if (IsColsSorted()){
    writer.WriteBypassBE(static_cast<uint32_t>(GetColIdsPayload()->GetSize()));
    GetColIdsPayload()->Write(writer);
  }
}

// -----------------------------------------------------------------------------

void SortedBinMatPayload::Write(core::Writer& writer) const {

}

// -----------------------------------------------------------------------------

}  // namespace genie::genotype

// -----------------------------------------------------------------------------