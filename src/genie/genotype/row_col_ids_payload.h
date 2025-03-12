/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#ifndef GENIE_SRC_GENIE_GENOTYPE_ROW_COL_IDS_PAYLOAD_H_
#define GENIE_SRC_GENIE_GENOTYPE_ROW_COL_IDS_PAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::genotype {

// ---------------------------------------------------------------------------------------------------------------------

class RowColIdsPayload {
  private:
   std::vector<uint8_t> payload_;

   // Only available when decoded, requires num_elements
   std::vector<uint64_t> row_col_ids_elements_;
 
  public:
   // Static function to calculate the number of bits per element
   static uint8_t ComputeNBitsPerElem(size_t num_elements);
 
   // Default constructor
   RowColIdsPayload();
 
   // Parameterized constructor with move semantics
   explicit RowColIdsPayload(std::vector<uint64_t>&& row_col_ids_elements);
 
   // Copy constructor
   RowColIdsPayload(const RowColIdsPayload& other);
 
   // Move constructor
   RowColIdsPayload(RowColIdsPayload&& other) noexcept;
 
   // Copy assignment operator
   RowColIdsPayload& operator=(const RowColIdsPayload& other);
 
   // Move assignment operator
   RowColIdsPayload& operator=(RowColIdsPayload&& other) noexcept;
 
   // Constructor from BitReader
   explicit RowColIdsPayload(
       genie::util::BitReader& reader,
       size_t size
   );
 
   // Getters
   [[nodiscard]] uint64_t GetNelements() const;
   [[nodiscard]] uint8_t GetNbitsPerElem() const;
   [[maybe_unused]] [[nodiscard]] const std::vector<uint64_t>& GetRowColIdsElements() const;
 
   // Setters
   [[maybe_unused]] void SetNelements(uint64_t nelements);
   [[maybe_unused]] void SetNbitsPerElem(uint32_t nbits_per_elem);
   [[maybe_unused]] void SetRowColIdsElements(std::vector<uint64_t>&& row_col_ids_elements);
 
   // Size functions
   [[nodiscard]] size_t GetSizeInBytes() const;
   [[maybe_unused]] [[nodiscard]] size_t PayloadSize() const;
   [[nodiscard]] size_t GetSize() const;
 
   // Function to write to writer
   void Write(util::BitWriter writer) const;
   void Write(core::Writer& writer) const;
 };

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::genotype

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SRC_GENIE_GENOTYPE_ROW_COL_IDS_PAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------