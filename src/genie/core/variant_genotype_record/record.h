/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_VARIANT_GENOTYPE_RECORD_H_
#define SRC_GENIE_CORE_RECORD_VARIANT_GENOTYPE_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/arrayType.h"
#include "genie/core/constants.h"
#include "genie/core/linked_record/linked_record.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Class representing an information field in a variant record
 *
 * Stores tag name, type, length and value information for variant annotations.
 */
class InfoField {
 private:
  uint8_t info_tag_len_;             //!< @brief
  std::string info_tag_;             //!< @brief
  uint8_t info_type_;                //!< @brief
  uint8_t info_array_len_;           //!< @brief
  std::vector<uint8_t> info_value_;  //!< @brief type??
};

/**
 * @brief Class representing a format field in a variant genotype record
 *
 * Stores information about sample data format, including type, values, and array dimensions.
 */
class FormatField {
 private:
  uint32_t sample_count_{};
  std::string format_;
  core::DataType type_;
  std::vector<std::vector<std::vector<uint8_t>>> value_;
  uint8_t array_length_{};

 public:
  /**
   * @brief Constructs a format field by reading from a bit stream
   * @param bitreader The bit reader to read from
   * @param sample_count The number of samples this format field applies to
   */
  FormatField(util::BitReader& bitreader, uint32_t sample_count);

  /**
   * @brief Default constructor for format_field
   */
  FormatField() = default;

  /**
   * @brief Gets the format string identifier
   * @return The format string
   */
  [[nodiscard]] std::string GetFormat() const {
    return format_;
  }

  /**
   * @brief Gets the data type of the format field
   * @return The data type
   */
  [[nodiscard]] core::DataType GetType() const {
    return type_;
  }

  /**
   * @brief Gets the data type of the format field
   * @return The data type
   */
  [[nodiscard]] uint32_t GetSampleCount() const {
    return sample_count_;
  }

  /**
   * @brief Gets the number of samples this format field applies to
   * @return The sample count
   */
  [[nodiscard]] const std::vector<std::vector<std::vector<uint8_t>>>& GetValue() const {
    return value_;
  }

  /**
   * @brief Gets the array length of the format field
   * @return The array length
   */
  [[nodiscard]] uint8_t GetArrayLength() const {
    return array_length_;
  }

  /**
   * @brief Constructs a format field with specified parameters
   * @param format The format string identifier
   * @param type The data type of the format field
   * @param arrayLength The length of the array for this format field
   */
  FormatField(std::string format, core::DataType type, uint8_t arrayLength)
      : sample_count_(0),
        format_(std::move(std::move(format))),
        type_(type),
        value_{},
        array_length_(arrayLength) {}

  /**
   * @brief Sets the number of samples
   * @param _sample_count The sample count to set
   */
  void SetSampleCount(uint32_t _sample_count) {
    sample_count_ = _sample_count;
  }

  /**
   * @brief Sets the format field values
   * @param _value The values to set
   */
  void SetValue(std::vector<std::vector<std::vector<uint8_t>>> _value) {
    value_ = std::move(_value);
  }
};

/**
 * @brief Class representing genotype information for a specific variant
 *
 * Contains information about samples, alleles, phasings, likelihoods and format fields
 * associated with a specific variant.
 */
class VariantGenotype {
 private:
  uint64_t variant_index_;      //!< @brief Variant index
  uint32_t sample_index_from_;  //!< @brief Starting sample index
  uint32_t sample_count_;       //!< @brief Number of samples

  std::vector<FormatField> format_;  //!< @brief Format fields

  std::vector<std::vector<int8_t>> alleles_;    //!< @brief Allele data
  std::vector<std::vector<uint8_t>> phasings_;  //!< @brief Phasing data
  std::vector<std::vector<uint32_t>> likelihoods_;  //!< @brief Likelihood data

  std::optional<LinkRecord> link_record_; //!< @brief Link record

 public:
 /**
   * @brief Default constructor
   */
 VariantGenotype();

 /**
   * @brief Parametrized constructor
   */
 VariantGenotype(
     uint64_t variant_index,
     uint32_t sample_index_from,
     uint32_t sample_count,
     std::vector<FormatField>&& format,
     std::vector<std::vector<int8_t>>&& alleles,
     std::vector<std::vector<uint8_t>>&& phasings,
     std::vector<std::vector<uint32_t>>&& likelihoods,
     const std::optional<LinkRecord>& link_record
 );

 /**
   * @brief Copy constructor
   * @param other VariantGenotype
   */
 VariantGenotype(const VariantGenotype& other);

 /**
   * @brief Move constructor
   * @param other VariantGenotype
   */
 VariantGenotype(VariantGenotype&& other) noexcept;

 /**
   * @brief Copy assignment operator
   * @param other VariantGenotype
   */
 VariantGenotype& operator=(const VariantGenotype& other);

 /**
   * @brief Move assignment operator
   * @param other VariantGenotype
   */
 VariantGenotype& operator=(VariantGenotype&& other) noexcept;

  /**
   * @brief Constructs a variant genotype by reading using BitReader
   * @param bitreader The bit reader to read from
   */
  explicit VariantGenotype(util::BitReader& bitreader);

  /**
   * @brief Constructs a variant genotype with specified parameters
   * @param variant_index The index of the variant
   * @param sample_index_from The starting sample index
   */
  VariantGenotype(uint64_t variant_index, uint32_t sample_index_from);

 // Getters


  /**
   * @brief Gets the variant index
   * @return The variant index
   */
  [[nodiscard]] uint64_t GetVariantIndex() const;

  /**
   * @brief Gets the starting sample index
   * @return The starting sample index
   */
  [[nodiscard]] uint32_t GetSampleIndexFrom() const;

  /**
   * @brief Gets the number of samples
   * @return The sample count
   */
  [[nodiscard]] uint32_t GetSampleCount() const;

 /**
  * @brief Get the format
  * @return Vector of FormatField
  */
 [[nodiscard]] const std::vector<FormatField>& GetFormat() const;

  /**
   * @brief Gets the number of format fields
   * @return The format field count
   */
  [[nodiscard]] uint8_t GetFormatCount() const;

  /**
   * @brief Checks if likelihood information is present
   * @return True if likelihood information exists, false otherwise
   */
  [[nodiscard]] bool IsGenotypePresent() const;

  /**
   * @brief Checks if likelihood information is present
   * @return True if likelihood information exists, false otherwise
   */
  [[nodiscard]] bool IsLikelihoodPresent() const;

  /**
   * @brief Gets the number of alleles per sample
   * @return The number of alleles per sample
   */
  [[nodiscard]] uint8_t GetNumberOfAllelesPerSample() const;

  /**
   * @brief Gets the allele information
   * @return Constant reference to the alleles
   */
  [[nodiscard]] const std::vector<std::vector<int8_t>>& GetAlleles() const;

  /**
   * @brief Gets the phasing information
   * @return Constant reference to the phasings
   */
  [[nodiscard]] const std::vector<std::vector<uint8_t>>& GetPhasing() const;

  /**
   * @brief Gets the number of likelihoods
   * @return The number of likelihoods
   */
  [[nodiscard]] uint8_t GetNumberOfLikelihoods() const;

  /**
   * @brief Gets the likelihood information
   * @return Constant reference to the likelihoods
   */
  [[nodiscard]] const std::vector<std::vector<uint32_t>>& GetLikelihoods() const;

  /**
   * @brief Gets the linked record
   * @return Constant reference to the link record
   */
  [[nodiscard]] bool GetLinkedRecord() const;

  /**
   * @brief Gets the linked record
   * @return Constant reference to the link record
   */
  [[nodiscard]] const std::optional<LinkRecord>& GetLinkRecord() const;

  /**
   * @brief Gets the format fields
   * @return Constant reference to the format fields
   */
  [[nodiscard]] const std::vector<FormatField>& GetFormats() const;

  /**
   * @brief Get the variant index
   * @param value to set the variant index
   */
  void SetVariantIndex(uint64_t value);

 /**
    * @brief Set the variant index
    */
 void SetSampleIndexFrom(uint32_t value);

 /**
    * @brief Set the number of sample count
    */
 void SetSampleCount(uint32_t value);

 /**
    * @brief Set the format value
    */
 void SetFormat(std::vector<FormatField> value);

 /**
    * @brief Set the format value
    */
 void SetFormat(std::vector<FormatField>&& value);

 /**
   * @brief Set the likelihood information
   */
  void SetLikelihoods(std::vector<std::vector<uint32_t>> value);

 /**
   * @brief Set the likelihood information
   */
 void SetLikelihoods(std::vector<std::vector<uint32_t>>&& value);

 /**
   * @brief Set the link record information
   */
 void SetLinkRecord(const std::optional<LinkRecord>& value);

  /**
   * @brief Sets the phasing information
   * @param value the phasing values to set
   */
 void SetPhasings(std::vector<std::vector<uint8_t>> value);

  /**
   * @brief Sets the phasing information
   * @param phasings The phasing values to set
   */
 void SetPhasings(std::vector<std::vector<uint8_t>>&& value);

  /**
   * @brief Sets the allele information
   * @param alleles The allele values to set
   */
 void SetAlleles(std::vector<std::vector<int8_t>> value);

 /**
   * @brief Sets the allele information
   * @param alleles The allele values to set
   */
 void SetAlleles(std::vector<std::vector<int8_t>>&& value);

  /**
   * @brief Sets the number of samples
   * @param sampleSize The sample count to set
   */
  void SetNumberOfSamples(uint32_t sampleSize);

 /**
 * @brief Get size of the VariantGenotype
 * @return size_t of VariantGenotype size
 */
  size_t GetSize() const;
};

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_VARIANT_GENOTYPE_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
