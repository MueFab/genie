/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_CONTACT_H_
#define SRC_GENIE_CORE_RECORD_CONTACT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <boost/optional/optional.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/record/linked_record/linked_record.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

/**
 * @brief Class representing a genomic contact record
 *
 * Stores information about genomic contacts, including chromosomes, positions,
 * and interaction counts between genomic regions.
 */
class ContactRecord {
 private:
  uint16_t sample_id_{};
  std::string sample_name_;
  uint32_t bin_size_{};
  uint8_t chr1_id_{};
  std::string chr1_name_;
  uint64_t chr1_length_{};
  uint8_t chr2_id_{};
  std::string chr2_name_;
  uint64_t chr2_length_{};
  std::vector<std::string> norm_count_names_;
  std::vector<uint64_t> start_pos1_;
  std::vector<uint64_t> end_pos1_;
  std::vector<uint64_t> start_pos2_;
  std::vector<uint64_t> end_pos2_;
  std::vector<uint32_t> counts_;
  std::vector<std::vector<double_t>> norm_counts_;
  std::optional<LinkRecord> link_record_;

 public:
  /**
   * @brief Default constructor
   */
  ContactRecord() = default;

  /**
   * @brief Move constructor
   * @param rec The contact record to move from
   */
  ContactRecord(ContactRecord&& rec) noexcept = default;

  /**
   * @brief Copy constructor
   * @param rec The contact record to copy from
   */
  ContactRecord(const ContactRecord& rec) = default;

  /**
   * @brief Constructs a contact record with specified parameters
   * @param sample_ID The sample ID
   * @param sample_name The sample name
   * @param bin_size The bin size
   * @param chr1_ID The first chromosome ID
   * @param chr1_name The first chromosome name
   * @param chr1_length The first chromosome length
   * @param chr2_ID The second chromosome ID
   * @param chr2_name The second chromosome name
   * @param chr2_length The second chromosome length
   * @param norm_count_names The names of normalization methods
   * @param start_pos1 The start positions on chromosome 1
   * @param end_pos1 The end positions on chromosome 1
   * @param start_pos2 The start positions on chromosome 2
   * @param end_pos2 The end positions on chromosome 2
   * @param counts The contact counts
   */
  ContactRecord(uint16_t sample_ID, std::string&& sample_name, uint32_t bin_size, uint8_t chr1_ID,
                std::string&& chr1_name, uint64_t chr1_length, uint8_t chr2_ID,
                std::string&& chr2_name, uint64_t chr2_length,
                std::vector<std::string>&& norm_count_names, std::vector<uint64_t>&& start_pos1,
                std::vector<uint64_t>&& end_pos1, std::vector<uint64_t>&& start_pos2,
                std::vector<uint64_t>&& end_pos2, std::vector<uint32_t>&& counts);

  /**
   * @brief Constructs a contact record by reading from a bit stream
   * @param reader The bit reader to read from
   */
  explicit ContactRecord(util::BitReader& reader);

  /**
   * @brief Default destructor
   */
  ~ContactRecord() = default;

  /**
   * @brief Copy assignment operator
   * @param rec The contact record to copy from
   * @return Reference to this contact record
   */
  ContactRecord& operator=(const ContactRecord& rec);

  /**
   * @brief Move assignment operator
   * @param rec The contact record to move from
   * @return Reference to this contact record
   */
  ContactRecord& operator=(ContactRecord&& rec) noexcept;

  /**
   * @brief Equality operator
   * @param other The contact record to compare with
   * @return True if records are equal, false otherwise
   */
  bool operator==(const ContactRecord& other);

  /**
   * @brief Transposes the contact matrix
   *
   * Swaps positions and chromosomes to represent the same contacts from
   * the opposite perspective.
   */
  void TransposeCM();

  /**
   * @brief Gets the sample ID
   * @return The sample ID
   */
  [[nodiscard]] uint16_t GetSampleID() const;

  /**
   * @brief Gets the sample name
   * @return Constant reference to the sample name
   */
  [[nodiscard]] const std::string& GetSampleName() const;

  /**
   * @brief Gets the bin size
   * @return The bin size
   */
  [[nodiscard]] uint32_t GetBinSize() const;

  /**
   * @brief Gets the first chromosome ID
   * @return The first chromosome ID
   */
  [[nodiscard]] uint8_t GetChr1ID() const;

  /**
   * @brief Gets the first chromosome name
   * @return Constant reference to the first chromosome name
   */
  [[nodiscard]] const std::string& GetChr1Name() const;

  /**
   * @brief Gets the first chromosome length
   * @return The first chromosome length
   */
  [[nodiscard]] uint64_t GetChr1Length() const;

  /**
   * @brief Gets the second chromosome ID
   * @return The second chromosome ID
   */
  [[nodiscard]] uint8_t GetChr2ID() const;

  /**
   * @brief Gets the second chromosome name
   * @return Constant reference to the second chromosome name
   */
  [[nodiscard]] const std::string& GetChr2Name() const;

  /**
   * @brief Gets the second chromosome length
   * @return The second chromosome length
   */
  [[nodiscard]] uint64_t GetChr2Length() const;

  /**
   * @brief Gets the number of contact entries
   * @return The number of entries
   */
  [[nodiscard]] uint64_t GetNumEntries() const;

  /**
   * @brief Gets the number of normalization methods
   * @return The number of normalization methods
   */
  [[nodiscard]] uint8_t GetNumNormCounts() const;

  /**
   * @brief Gets the normalization method names
   * @return Constant reference to the normalization method names
   */
  [[nodiscard]] const std::vector<std::string>& GetNormCountNames() const;

  /**
   * @brief Gets the start positions on chromosome 1
   * @return Constant reference to the start positions on chromosome 1
   */
  [[nodiscard]] const std::vector<uint64_t>& GetStartPos1() const;

  /**
   * @brief Gets the end positions on chromosome 1
   * @return Constant reference to the end positions on chromosome 1
   */
  [[nodiscard]] const std::vector<uint64_t>& GetEndPos1() const;

  /**
   * @brief Gets the start positions on chromosome 2
   * @return Constant reference to the start positions on chromosome 2
   */
  [[nodiscard]] const std::vector<uint64_t>& GetStartPos2() const;

  /**
   * @brief Gets the end positions on chromosome 2
   * @return Constant reference to the end positions on chromosome 2
   */
  [[nodiscard]] const std::vector<uint64_t>& GetEndPos2() const;

  /**
   * @brief Gets the contact counts
   * @return Constant reference to the contact counts
   */
  [[nodiscard]] const std::vector<uint32_t>& GetCounts() const;

  /**
   * @brief Gets the normalized counts
   * @return Constant reference to the normalized counts
   */
  [[nodiscard]] const std::vector<std::vector<double_t>>& GetNormCounts() const;

  /**
   * @brief Sets the sample ID
   * @param sample_ID The sample ID to set
   */
  void SetSampleId(uint16_t _sample_ID);

  /**
   * @brief Sets the sample name
   * @param sample_name The sample name to set
   */
  void SetSampleName(std::string&& _sample_name);

  /**
   * @brief Sets the bin size
   * @param bin_size The bin size to set
   */
  void SetBinSize(uint32_t _bin_size);

  /**
   * @brief Sets the first chromosome ID
   * @param chr1_ID The first chromosome ID to set
   */
  void SetChr1ID(uint8_t _chr1_ID);

  /**
   * @brief Sets the first chromosome name
   * @param chr1_name The first chromosome name to set
   */
  void SetChr1Name(std::string&& _chr1_name);

  /**
   * @brief Sets the first chromosome length
   * @param chr1_len The first chromosome length to set
   */
  void SetChr1Length(uint64_t _chr1_len);

  /**
   * @brief Sets the second chromosome ID
   * @param chr2_ID The second chromosome ID to set
   */
  void SetChr2ID(uint8_t _chr2_ID);

  /**
   * @brief Sets the second chromosome name
   * @param chr2_name The second chromosome name to set
   */
  void SetChr2Name(std::string&& _chr2_name);

  /**
   * @brief Sets the second chromosome length
   * @param chr2_len The second chromosome length to set
   */
  void SetChr2Length(uint64_t _chr2_len);

  /**
   * @brief Sets the contact matrix values
   * @param start_pos1 The start positions on chromosome 1
   * @param end_pos1 The end positions on chromosome 1
   * @param start_pos2 The start positions on chromosome 2
   * @param end_pos2 The end positions on chromosome 2
   * @param counts The contact counts
   */
  void SetCMValues(std::vector<uint64_t>&& start_pos1, std::vector<uint64_t>&& end_pos1,
                   std::vector<uint64_t>&& start_pos2, std::vector<uint64_t>&& end_pos2,
                   std::vector<uint32_t>&& counts);

  /**
   * @brief Writes the contact record to a bit writer
   * @param writer The bit writer to write to
   */
  void Write(util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_CONTACT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
