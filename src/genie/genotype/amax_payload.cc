/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
 */

// -----------------------------------------------------------------------------

#include "amax_payload.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::genotype {

// -----------------------------------------------------------------------------

AmaxPayload::AmaxPayload()
    : amax_elements_(), nbits_per_elem_(0) {}

// -----------------------------------------------------------------------------

AmaxPayload::AmaxPayload(std::vector<uint64_t>&& amax_elements, 
                         std::optional<uint8_t> nbits_per_elem)
    : amax_elements_{std::move(amax_elements)} {
  UTILS_DIE_IF(amax_elements_.empty(), "amax_elements cannot be empty!");

  if (nbits_per_elem.has_value()) {
    nbits_per_elem_ = nbits_per_elem.value();
  } else {
    nbits_per_elem_ = ComputeNbitsPerElement(amax_elements_);
  }
}

// -----------------------------------------------------------------------------

AmaxPayload::AmaxPayload(
    const AmaxPayload& other
): amax_elements_(other.amax_elements_),
   nbits_per_elem_(other.nbits_per_elem_) {}

// -----------------------------------------------------------------------------

AmaxPayload::AmaxPayload(AmaxPayload&& other) noexcept
    : amax_elements_(std::move(other.amax_elements_)),
      nbits_per_elem_(other.nbits_per_elem_) {}

// -----------------------------------------------------------------------------

AmaxPayload& AmaxPayload::operator=(const AmaxPayload& other) {
  if (this != &other) {
    nbits_per_elem_ = other.nbits_per_elem_;
    amax_elements_ = other.amax_elements_;
  }
  return *this;
}

// -----------------------------------------------------------------------------

AmaxPayload& AmaxPayload::operator=(AmaxPayload&& other) noexcept {
  if (this != &other) {
    nbits_per_elem_ = other.nbits_per_elem_;
    amax_elements_ = std::move(other.amax_elements_);
  }
  return *this;
}

// -----------------------------------------------------------------------------

bool AmaxPayload::operator==(const AmaxPayload& other) noexcept {
  return (nbits_per_elem_ && other.nbits_per_elem_ &&
          amax_elements_ == other.amax_elements_);
}


// -----------------------------------------------------------------------------

AmaxPayload::AmaxPayload(genie::util::BitReader& reader) {
  auto nelems = reader.Read<uint32_t>();
  nbits_per_elem_ = reader.Read<uint8_t>();

  amax_elements_.resize(nelems);
  for (size_t i = 0; i < nelems; ++i) {
    auto is_one_flag = reader.Read<bool>(1);
    if (is_one_flag) {
      amax_elements_[i] = reader.Read<uint64_t>(nbits_per_elem_) + 2;
    } else {
      amax_elements_[i] = 1;
    }
  }

  reader.FlushHeldBits();
}

// -----------------------------------------------------------------------------

size_t AmaxPayload::GetNElems() const {
  return amax_elements_.size();
}

// -----------------------------------------------------------------------------

uint8_t AmaxPayload::GetNBitsPerElem() const {
  return nbits_per_elem_;
}

// -----------------------------------------------------------------------------

const std::vector<uint64_t>& AmaxPayload::GetAmaxElements() const {
  return amax_elements_;
}

// -----------------------------------------------------------------------------

[[maybe_unused]] void AmaxPayload::SetNbitsPerElem(uint8_t nbits_per_elem) {
  nbits_per_elem_ = nbits_per_elem;
}

// -----------------------------------------------------------------------------

[[maybe_unused]] void AmaxPayload::SetAmaxElements(std::vector<uint64_t>&& amax_elements) {
  amax_elements_ = std::move(amax_elements);
}

// -----------------------------------------------------------------------------

size_t AmaxPayload::GetSize() const {
  std::stringstream bitstream;
  genie::util::BitWriter writer(&bitstream);
  Write(writer);

  size_t payload_size = bitstream.str().size();
  return payload_size;

//  size_t size_in_bits = 32 + 8;  // size of nelems_ + size of nbits_per_elem_
//  size_in_bits += amax_elements_.size(); // Bits for the flag
//  for (unsigned long amax_element : amax_elements_) {
//    if (amax_element > 1) {
//      size_in_bits += nbits_per_elem_;  // nbits_per_entry == nbits_per_elem_
//    }
//  }
//  uint8_t remain = 8 - (size_in_bits & 8);
//
//  return (size_in_bits + remain) >> 3;
}

// -----------------------------------------------------------------------------

void AmaxPayload::Write(util::BitWriter& writer) const {
  UTILS_DIE_IF(!writer.IsByteAligned(), "Byte is not aligned!");
  writer.WriteBypassBE(static_cast<uint32_t>(GetNElems()));
  writer.WriteBypassBE(static_cast<uint8_t>(GetNBitsPerElem()));

  for (auto amax_element : GetAmaxElements()) {
    auto is_one_flag = amax_element > 1;
    writer.WriteBits(is_one_flag, 1);
    if (is_one_flag) {
      writer.WriteBits(amax_element-2, GetNBitsPerElem());
    }
  }

  writer.FlushBits();
}

// -----------------------------------------------------------------------------

void AmaxPayload::Write(core::Writer& writer) const {
  writer.Write(GetNElems(), 32);
  writer.Write(GetNBitsPerElem(), 8);
  for (auto amax_element : GetAmaxElements()) {
    auto is_one_flag = amax_element > 1;
    writer.Write(is_one_flag, 1);
    if (is_one_flag) {
      writer.Write(amax_element-2, GetNBitsPerElem());
    }
  }

  writer.Flush();
}

// -----------------------------------------------------------------------------

uint8_t AmaxPayload::ComputeNbitsPerElement(const std::vector<uint64_t>& amax_elements) {
    UTILS_DIE_IF(amax_elements.empty(), "amax_elements is empty!");
    uint64_t max_val = *std::max_element(amax_elements.begin(), amax_elements.end());
    uint64_t min_val = *std::min_element(amax_elements.begin(), amax_elements.end());

    UTILS_DIE_IF(max_val == 0, "Maximum value of amax_elements cannot be 0!");
    UTILS_DIE_IF(min_val == 0, "Minimum value of amax_elements cannot be 0!");

    if (max_val == 1){
        return 0;
    }
    uint64_t adjusted_max = max_val - 2;

    // Compute bit length
    auto nbits = static_cast<uint8_t>(std::ceil(std::log2(adjusted_max + 1)));
    return nbits;
}

// -----------------------------------------------------------------------------

}  // namespace genotype::genie

// -----------------------------------------------------------------------------