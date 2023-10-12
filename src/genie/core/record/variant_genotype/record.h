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
#include <string>
#include <utility>
#include <vector>
#include <optional>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/core/constants.h"
#include "genie/core/record/linked_record/linked_record.h"

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
 public:
    //    uint8_t len;
    //    std::string format;
    //    uint8_t type;
    //    uint8_t array_len;
    //    std::vector<std::vector<std::vector<uint8_t>>> value_array;
    //
    //    format_field() : len(0), format(""), type(0), array_len(0), value_array(0) {}
    //
    //    format_field(uint8_t len, std::string format, uint8_t type, uint8_t array_len,
    //                std::vector<std::vector<std::vector<uint8_t>>> value)
    //        : len(len), format(format), type(type), array_len(array_len), value_array(value) {}
    //
    //    void write(util::BitReader& reader, uint32_t sample_count);
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

    std::vector<std::vector<int8_t>> alleles;  //!< @brief
    std::vector<std::vector<int8_t>> phasings;  //!< @brief

    std::vector<std::vector<uint32_t>> likelihoods;  //!< @brief

    std::optional<LinkRecord> link_record;

 public:

    explicit VariantGenotype(util::BitReader& bitreader);

    uint64_t getVariantIndex() const;
    uint32_t getStartSampleIndex() const;
    uint32_t getNumSamples() const;

    uint8_t getFormatCount() const;
    //    const std::vector<format_field>& getFormat() const;
    bool isGenotypePresent() const;
    bool isLikelihoodPresent() const;
    uint8_t getNumberOfAllelesPerSample() const;
    const std::vector<std::vector<int8_t>>& getAlleles() const;
    const std::vector<std::vector<int8_t>>& getPhasing() const;
    uint8_t getNumberOfLikelihoods() const;
    const std::vector<std::vector<uint32_t>>& getLikelihoods() const;
    bool getLinkedRecord() const;
    const LinkRecord& getLinkRecord() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_VARIANT_GENOTYPE_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
