/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include "row_col_ids_payload.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::genotype {

// -----------------------------------------------------------------------------

// Default constructor
RowColIdsPayload::RowColIdsPayload() = default;

// -----------------------------------------------------------------------------

// Parameterized constructor with move semantics
RowColIdsPayload::RowColIdsPayload(std::vector<uint32_t>&& row_col_ids_elements)
    : row_col_ids_elements_(std::move(row_col_ids_elements)) {
}

// -----------------------------------------------------------------------------

// Copy constructor
RowColIdsPayload::RowColIdsPayload(const RowColIdsPayload& other)
    : row_col_ids_elements_(other.row_col_ids_elements_) {
}

// -----------------------------------------------------------------------------

// Move constructor
RowColIdsPayload::RowColIdsPayload(RowColIdsPayload&& other) noexcept
    : row_col_ids_elements_(std::move(other.row_col_ids_elements_)) {
}

// -----------------------------------------------------------------------------

// Copy assignment operator
RowColIdsPayload& RowColIdsPayload::operator=(const RowColIdsPayload& other) {
  if (this != &other) {
    row_col_ids_elements_ = other.row_col_ids_elements_;
  }
  return *this;
}

// -----------------------------------------------------------------------------

// Move assignment operator
RowColIdsPayload& RowColIdsPayload::operator=(RowColIdsPayload&& other) noexcept {
  if (this != &other) {
    row_col_ids_elements_ = std::move(other.row_col_ids_elements_);
  }
  return *this;
}

// -----------------------------------------------------------------------------

// Constructor from BitReader
RowColIdsPayload::RowColIdsPayload(genie::util::BitReader& reader, size_t num_elements) {
//    payload_.resize(num_elements);
  auto n_bits_per_elem = ComputeNBitsPerElem(num_elements);
  for (auto i = 0u; i < num_elements; i++){
    row_col_ids_elements_.push_back(static_cast<uint32_t>(reader.ReadBits(n_bits_per_elem)));
  }
  reader.FlushHeldBits();
}

// -----------------------------------------------------------------------------

// Equality operator
bool RowColIdsPayload::operator==(const RowColIdsPayload& other) const {
  return row_col_ids_elements_ == other.row_col_ids_elements_;
//  return payload_ == other.payload_ && row_col_ids_elements_ == other.row_col_ids_elements_;
}

// -----------------------------------------------------------------------------

// Inequality operator
bool RowColIdsPayload::operator!=(const RowColIdsPayload& other) const {
  return !(*this == other);
}

// -----------------------------------------------------------------------------

uint32_t RowColIdsPayload::GetNelements() const {
  return static_cast<uint32_t>(row_col_ids_elements_.size());
}

// -----------------------------------------------------------------------------

uint8_t RowColIdsPayload::GetNbitsPerElem() const {
  return ComputeNBitsPerElem(row_col_ids_elements_.size());
}

// -----------------------------------------------------------------------------

[[maybe_unused]] const std::vector<uint32_t>& RowColIdsPayload::GetRowColIdsElements() const {
  return row_col_ids_elements_;
}

// -----------------------------------------------------------------------------

//[[maybe_unused]] void RowColIdsPayload::SetNelements(uint64_t nelements) {
//  row_col_ids_elements_.resize(nelements);
//}

// -----------------------------------------------------------------------------

//[[maybe_unused]] void RowColIdsPayload::SetNbitsPerElem(uint32_t nbits_per_elem) {
//  UTILS_DIE("Forbidden call! nbits_per_elem is computed!");
//}

// -----------------------------------------------------------------------------

[[maybe_unused]] void RowColIdsPayload::SetRowColIdsElements(std::vector<uint32_t>&& row_col_ids_elements) {
  row_col_ids_elements_ = std::move(row_col_ids_elements);
}

// -----------------------------------------------------------------------------

size_t RowColIdsPayload::GetSizeInBytes() const {
  return GetSize();
}

// -----------------------------------------------------------------------------

[[maybe_unused]] size_t RowColIdsPayload::PayloadSize() const {
  return GetSizeInBytes();
}

// -----------------------------------------------------------------------------

size_t RowColIdsPayload::GetSize() const {
  // auto num_elements = row_col_ids_elements_.size();
  // auto nbits_per_elem = ComputeNBitsPerElem(num_elements);
  // auto size_in_bits = num_elements * nbits_per_elem;
  // auto remain = 8 - (size_in_bits & 8);
  // return (size_in_bits + remain) >> 3;

  auto num_elements = row_col_ids_elements_.size();
  auto nbits_per_elem = ComputeNBitsPerElem(num_elements);
  auto size_in_bits = num_elements * nbits_per_elem;
  return (size_in_bits + 7) / 8; // Rounds up to the nearest byte
}

// -----------------------------------------------------------------------------

void RowColIdsPayload::Write(util::BitWriter writer) const {
  auto nbits_per_elem = GetNbitsPerElem();
  for (auto i = 0u; i< GetNelements(); i++){
    writer.WriteBits(row_col_ids_elements_[i], nbits_per_elem);
  }

  writer.FlushBits();
}

// -----------------------------------------------------------------------------

void RowColIdsPayload::Write(core::Writer& writer) const {
  auto nbits_per_elem = GetNbitsPerElem();
  for (auto i = 0u; i< GetNelements(); i++){
    writer.Write(row_col_ids_elements_[i], nbits_per_elem);
  }

  writer.Flush();
}

// -----------------------------------------------------------------------------

// Static method implementation
uint8_t RowColIdsPayload::ComputeNBitsPerElem(size_t num_elements) {
  UTILS_DIE_IF(num_elements == 0, "num_elements cannot be 0!");

  return static_cast<uint8_t>(std::ceil(std::log2(num_elements + 1)));
//  return static_cast<uint8_t>(log2(ceil(static_cast<double>(num_elements))));
}

// -----------------------------------------------------------------------------

}  // namespace genie::genotype

// -----------------------------------------------------------------------------