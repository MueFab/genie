/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
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
#include "genie/core/record/linked_record/linked_record.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

/**
 *  @brief
 */
class info_field {
 private:
    uint8_t info_tag_len;             //!< @brief
    std::string info_tag;             //!< @brief
    uint8_t info_type;                //!< @brief
    uint8_t info_array_len;           //!< @brief
    std::vector<uint8_t> info_value;  //!< @brief type??
};

class format_field {
 private:
    uint32_t sample_count;
    std::string format;
    core::DataType type;
    std::vector<std::vector<std::vector<uint8_t>>> value;
    uint8_t arrayLength;

 public:
    format_field(util::BitReader& bitreader, uint32_t _sample_count);
    format_field() = default;
    std::string getFormat() const { return format; }
    core::DataType getType() const { return type; }
    uint32_t getSampleCount() const { return sample_count; }
    const std::vector<std::vector<std::vector<uint8_t>>>& getValue() const { return value; }
    uint8_t getArrayLength() const { return arrayLength; }

    format_field(std::string _format, core::DataType _type, uint8_t _arrayLength)
        : sample_count(0), format(_format), type(_type), value{}, arrayLength(_arrayLength) {}
    void setSampleCount(uint32_t _sample_count) { sample_count = _sample_count; }
    void setValue(std::vector<std::vector<std::vector<uint8_t>>> _value) { value = _value; }
};

/**
 *  @brief
 */
class VariantGenotype {
 private:
  uint64_t variant_index_;      //!< @brief Variant index
  uint32_t sample_index_from_;  //!< @brief Starting sample index
  uint32_t sample_count_;       //!< @brief Number of samples

  std::vector<format_field> format_;  //!< @brief Format fields

  std::vector<std::vector<int8_t>> alleles_;    //!< @brief Allele data
  std::vector<std::vector<uint8_t>> phasings_;  //!< @brief Phasing data
  std::vector<std::vector<uint32_t>> likelihoods_;  //!< @brief Likelihood data

  std::optional<LinkRecord> link_record_;  //!< @brief Link record

 public:
    // Default constructor
    VariantGenotype();

    // Parameterized constructor
    VariantGenotype(
        uint64_t variant_index,
        uint32_t sample_index_from,
        uint32_t sample_count,
        std::vector<format_field>&& format,
        std::vector<std::vector<int8_t>>&& alleles,
        std::vector<std::vector<uint8_t>>&& phasings,
        std::vector<std::vector<uint32_t>>&& likelihoods,
        const std::optional<LinkRecord>& link_record
    );

    // Copy constructor
    VariantGenotype(const VariantGenotype& other);

    // Move constructor
    VariantGenotype(VariantGenotype&& other) noexcept;

    // Copy assignment operator
    VariantGenotype& operator=(const VariantGenotype& other);

    // Move assignment operator
    VariantGenotype& operator=(VariantGenotype&& other) noexcept;

    // Constructor from BitReader
    explicit VariantGenotype(util::BitReader& reader);

    // Getters
    uint64_t GetVariantIndex() const;
    uint32_t GetSampleIndexFrom() const;
    uint32_t GetSampleCount() const;
    const std::vector<format_field>& GetFormat() const;
    const std::vector<std::vector<int8_t>>& GetAlleles() const;
    const std::vector<std::vector<uint8_t>>& GetPhasing() const;
    const std::vector<std::vector<uint32_t>>& GetLikelihoods() const;
    const std::optional<LinkRecord>& GetLinkRecord() const;

    // Setters
    void SetVariantIndex(uint64_t value);
    void SetSampleIndexFrom(uint32_t value);
    void SetSampleCount(uint32_t value);
    void SetFormat(std::vector<format_field> value);
    void SetFormat(std::vector<format_field>&& value);
    void SetAlleles(std::vector<std::vector<int8_t>> value);
    void SetAlleles(std::vector<std::vector<int8_t>>&& value);
    void SetPhasings(std::vector<std::vector<uint8_t>> value);
    void SetPhasings(std::vector<std::vector<uint8_t>>&& value);
    void SetLikelihoods(std::vector<std::vector<uint32_t>> value);
    void SetLikelihoods(std::vector<std::vector<uint32_t>>&& value);
    void SetLinkRecord(const std::optional<LinkRecord>& value);

    // Additional Getter
    uint8_t GetFormatCount() const;
    bool IsGenotypePresent() const;
    uint8_t GetNumberOfAllelesPerSample() const;
    bool IsLikelihoodPresent() const;
    uint8_t GetNumberOfLikelihoods() const;
    bool GetLinkedRecord() const;

    // Size calculation
    size_t GetSize() const;
};

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_VARIANT_GENOTYPE_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
