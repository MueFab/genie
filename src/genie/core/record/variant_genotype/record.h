/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_VARIANT_GENOTYPE_H_
#define SRC_GENIE_CORE_RECORD_VARIANT_GENOTYPE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace variant_genotype {

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

/**
 *  @brief
 */

/**
 *  @brief
 */
class linked_record {
 public:
    uint8_t link_name_len;     //!< @brief
    std::string link_name;     //!< @brief
    uint8_t reference_box_ID;  //!< @brief
};

class FormatField {
 public:
    uint8_t len;
    std::string format;
    uint8_t type;
    uint8_t array_len;
    std::vector<std::vector<std::vector<uint8_t>>> value_array;

    FormatField() : len(0), format(""), type(0), array_len(0), value_array(0){};

    FormatField(uint8_t len, std::string format, uint8_t type, uint8_t array_len,
                std::vector<std::vector<std::vector<uint8_t>>> value)
        : len(len), format(format), type(type), array_len(array_len), value_array(value) {}

    void write(util::BitReader& reader, uint32_t sample_count);
};

/**
 *  @brief
 */
class Record {
 private:
    // util::BitReader& reader;

    uint64_t variant_index;      //!< @brief
    uint32_t sample_index_from;  //!< @brief
    uint32_t sample_count;       //!< @brief

    uint8_t format_count;             //!< @brief
    std::vector<FormatField> format;  //!< @brief

    uint8_t genotype_present;    //!< @brief
    uint8_t likelihood_present;  //!< @brief

    uint8_t n_alleles_per_sample;               //!< @brief
    std::vector<std::vector<uint8_t>> alleles;  //!< @brief
    std::vector<std::vector<uint8_t>> phasing;  //!< @brief

    uint8_t n_likelihoods;                        //!< @brief
    std::vector<std::vector<float>> likelihoods;  //!< @brief

    uint8_t linked_record;     //!< @brief
    uint8_t link_name_len;     //!< @brief
    std::string link_name;     //!< @brief
    uint8_t reference_box_ID;  //!< @brief

 public:
    /**
     * @brief
     */
    Record()
        : variant_index(0),
          sample_index_from(0),
          sample_count(0),
          format_count(0),
          format(0),
          genotype_present(0),
          likelihood_present(0),
          n_alleles_per_sample(1),
          alleles(0),
          phasing(0),
          n_likelihoods(0),
          likelihoods(0),
          linked_record(0),
          link_name_len(0),
          link_name(""),
          reference_box_ID(0) {}

    Record(util::BitReader& reader);
    void read(util::BitReader& reader);
    void write(std::ostream& outputfile) const;

    uint64_t getVariantIndex() const;
    uint32_t getStartSampleIndex() const;

    uint32_t getSampleCount() const;
    uint8_t getFormatCount() const;
    std::vector<FormatField> getFormat() const;
    bool isGenotypePresent() const;
    bool isLikelihoodPresent() const;
    uint8_t getNumberOfAllelesPerSample() const;
    std::vector<std::vector<uint8_t>> getAlleles() const;
    std::vector<std::vector<uint8_t>> getPhasing() const;
    uint8_t getNumberOfLikelihoods() const;
    std::vector<std::vector<float>> getLikelihoods() const;
    bool isLinkedRecord() const;
    std::string getLinkName() const;
    uint8_t getReferenceBoxID() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace variant_genotype
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_VARIANT_GENOTYPE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
