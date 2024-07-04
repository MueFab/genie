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
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

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

    std::string getFormat() const { return format; }
    core::DataType getType() const { return type; }
    uint32_t getSampleCount() const { return sample_count; }
    const std::vector<std::vector<std::vector<uint8_t>>>& getValue() const { return value; }
    uint8_t getArrayLength() const { return arrayLength; }
};

/**
 *  @brief
 */
class VariantGenotype {
 private:
    uint64_t variant_index;      //!< @brief
    uint32_t sample_index_from;  //!< @brief
    uint32_t sample_count;       //!< @brief

    std::vector<format_field> format;  //!< @brief

    std::vector<std::vector<int8_t>> alleles;    //!< @brief
    std::vector<std::vector<uint8_t>> phasings;  //!< @brief

    std::vector<std::vector<uint32_t>> likelihoods;  //!< @brief

    std::optional<LinkRecord> link_record;

 public:
    explicit VariantGenotype(util::BitReader& bitreader);
    VariantGenotype(uint64_t variant_index, uint32_t sample_index_from);
    uint64_t getVariantIndex() const;
    uint32_t getStartSampleIndex() const;
    uint32_t getNumSamples() const;

    uint8_t getFormatCount() const;
    //    const std::vector<format_field>& getFormat() const;
    bool isGenotypePresent() const;
    bool isLikelihoodPresent() const;
    uint8_t getNumberOfAllelesPerSample() const;
    const std::vector<std::vector<int8_t>>& getAlleles() const;
    const std::vector<std::vector<uint8_t>>& getPhasing() const;
    uint8_t getNumberOfLikelihoods() const;
    const std::vector<std::vector<uint32_t>>& getLikelihoods() const;
    bool getLinkedRecord() const;
    const LinkRecord& getLinkRecord() const;
    const std::vector<format_field>& getFormats() const;

    void setLikelihood(std::vector<std::vector<uint32_t>> _likelihoods) { likelihoods = _likelihoods; }
    void setPhasings(std::vector<std::vector<uint8_t>> _phasings) { phasings = _phasings; };
    void setAlleles(std::vector<std::vector<int8_t>> _alleles) { alleles = _alleles; }
    void setNumberOfSamples(uint32_t sampleSize) { sample_count = sampleSize; };
};

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_VARIANT_GENOTYPE_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
